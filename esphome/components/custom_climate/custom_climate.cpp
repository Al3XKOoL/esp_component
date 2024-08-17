#include "custom_climate.h"
#include <cstdarg>
#include <cstdio>
#include "esphome/core/log.h"
#include "esphome/components/climate/climate.h"

namespace custom_climate {

using namespace esphome::climate;

static const char *const TAG = "custom_climate";

int conteo_encendidos_{0};
int64_t tiempo_encendida_{0};

void CustomClimate::log_mensaje(const char* nivel, const char* formato, ...) {
    va_list args;
    va_start(args, formato);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), formato, args);
    va_end(args);
    esphome::ESP_LOGD(TAG, "\033[1;31m%s: %s\033[0m", nivel, buffer);  // Rojo para todos los niveles
}

void CustomClimate::setup() {
  this->mode = CLIMATE_MODE_OFF;
  this->target_temperature = 37.0;
  this->current_temperature = get_current_temperature();
  this->publish_state();
}

void CustomClimate::loop() {
  unsigned long tiempo_actual = millis();

  if (tiempo_actual - ultimo_tiempo_verificacion_ >= intervalo_segundos_ * 1000) {
    ultimo_tiempo_verificacion_ = tiempo_actual;

    log_mensaje("DEBUG", "Ejecutando loop()");

    // Actualizar lecturas de todos los sensores
    float temp_sol = sensor_temp_sol_->state;
    float temp_agua = sensor_temp_agua_->state;
    float temp_salida = sensor_temp_salida_->state;

    // Actualizar la temperatura actual del climate (usando temp_agua)
    if (!std::isnan(temp_agua)) {
      this->current_temperature = temp_agua;
    }

    log_mensaje("DEBUG", "Temperaturas - Sol: %.2f, Agua: %.2f, Salida: %.2f", temp_sol, temp_agua, temp_salida);

    // Solo realizar comprobaciones si el modo es HEAT
    if (this->mode == CLIMATE_MODE_HEAT) {
      // Obtener tiempo actual
      int64_t timestamp_actual = 0;
      if (tiempo_homeassistant_ != nullptr) {
        timestamp_actual = tiempo_homeassistant_->now().timestamp;
      }
      if (timestamp_actual == 0 && tiempo_sntp_ != nullptr) {
        timestamp_actual = tiempo_sntp_->now().timestamp;
      }

      if (espera_) {
        if (timestamp_actual >= tiempo_espera_fin_) {
          espera_ = false;
          log_mensaje("DEBUG", "Reanudando verificaciones después de espera de 5 minutos.");
        } else {
          log_mensaje("DEBUG", "En espera hasta %lld", tiempo_espera_fin_);
          this->publish_state();
          return;
        }
      }

      bool estado_bomba_actual = interruptor_bomba_->state;

      if (temp_sol > (temp_agua + diferencia_alta_) && temp_agua < this->target_temperature) {
        if (!estado_bomba_actual) {
          interruptor_bomba_->turn_on();
          conteo_encendidos_++;
          tiempo_inicio_ = timestamp_actual;
          log_mensaje("DEBUG", "Bomba encendida debido a la temperatura adecuada");
        } else {
          log_mensaje("DEBUG", "Bomba ya está encendida");
        }
      } else {
        if (estado_bomba_actual) {
          interruptor_bomba_->turn_off();
          int64_t tiempo_total_encendido = timestamp_actual - tiempo_inicio_;
          tiempo_encendida_ += tiempo_total_encendido;
          log_mensaje("DEBUG", "Bomba apagada debido a temperatura inadecuada");
          log_mensaje("WARN", "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);
          espera_ = true;
          tiempo_espera_fin_ = timestamp_actual + 300; // 5 minutos en segundos
          this->publish_state();
          return;
        } else {
          log_mensaje("DEBUG", "Bomba ya está apagada");
          this->publish_state();
          return;
        }
      }

      if (estado_bomba_actual && temp_salida < (temp_agua + 1)) {
        interruptor_bomba_->turn_off();
        int64_t tiempo_total_encendido = timestamp_actual - tiempo_inicio_;
        tiempo_encendida_ += tiempo_total_encendido;
        log_mensaje("DEBUG", "Bomba apagada debido a temperatura de salida insuficiente");
        log_mensaje("WARN", "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);
        espera_ = true;
        tiempo_espera_fin_ = timestamp_actual + 300; // 5 minutos en segundos
        this->publish_state();
        return;
      }

      if (estado_bomba_actual) {
        int64_t tiempo_transcurrido = timestamp_actual - tiempo_inicio_;
        log_mensaje("WARN", "Tiempo transcurrido de funcionamiento de la bomba: %02d:%02d:%02d",
                    (int)(tiempo_transcurrido / 3600),
                    (int)((tiempo_transcurrido % 3600) / 60),
                    (int)(tiempo_transcurrido % 60));
      }

      log_mensaje("WARN", "Diferencia Sol-Agua: %.2f°C", temp_sol - temp_agua);
      log_mensaje("WARN", "Diferencia Salida-Agua: %.2f°C", temp_salida - temp_agua);
      log_mensaje("WARN", "Estado de la bomba: %d", estado_bomba_actual);
      log_mensaje("DEBUG", "Conteo de encendidos de la bomba: %d", conteo_encendidos_);
    } else {
      // Si no está en modo HEAT, asegurarse de que la bomba esté apagada
      if (interruptor_bomba_->state) {
        interruptor_bomba_->turn_off();
        log_mensaje("DEBUG", "Bomba apagada porque el modo HEAT está desactivado");
      }
    }

    this->publish_state();
  }
}

esphome::climate::ClimateTraits CustomClimate::traits() {
  auto traits = esphome::climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_visual_min_temperature(temperatura_visual_minima_);
  traits.set_visual_max_temperature(temperatura_visual_maxima_);
  traits.set_visual_temperature_step(0.1);
  traits.add_supported_mode(CLIMATE_MODE_OFF);
  traits.add_supported_mode(CLIMATE_MODE_HEAT);
  return traits;
}

void CustomClimate::control(const esphome::climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    auto new_mode = *call.get_mode();
    if (new_mode != this->mode) {
      this->mode = new_mode;
      if (this->mode == CLIMATE_MODE_OFF) {
        interruptor_bomba_->turn_off();
        log_mensaje("DEBUG", "Bomba apagada debido a cambio a modo OFF");
      } else if (this->mode == CLIMATE_MODE_HEAT) {
        log_mensaje("DEBUG", "Modo HEAT activado");
      }
    }
  }
  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
  }
  this->publish_state();
}

float CustomClimate::get_current_temperature() {
  if (sensor_temp_agua_ != nullptr && !std::isnan(sensor_temp_agua_->state)) {
    return sensor_temp_agua_->state;
  } else {
    log_mensaje("ERROR", "El sensor de temperatura del agua no está disponible o devuelve NaN.");
    return NAN;
  }
}

}  // namespace custom_climate
