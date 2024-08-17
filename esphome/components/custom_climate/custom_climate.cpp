#include "custom_climate.h"
#include <cstdarg>
#include <cstdio>
#include "esphome/core/log.h"

namespace custom_climate {

static const char *const TAG = "custom_climate";

void CustomClimate::log_mensaje(const char* nivel, const char* formato, ...) {
    va_list args;
    va_start(args, formato);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), formato, args);
    va_end(args);
    esphome::ESP_LOGD(TAG, "%s: %s", nivel, buffer);
}

void CustomClimate::setup() {
  // Inicialización si es necesaria
}

void CustomClimate::loop() {
  unsigned long tiempo_actual = millis();

  if (tiempo_actual - ultimo_tiempo_verificacion_ >= intervalo_segundos_ * 1000) {
    ultimo_tiempo_verificacion_ = tiempo_actual;

    log_mensaje("DEBUG", "Ejecutando loop()");

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
        return;
      }
    }

    bool estado_bomba_actual = interruptor_bomba_->state;

    if (sensor_temp_sol_->state > (sensor_temp_agua_->state + diferencia_alta_) && sensor_temp_agua_->state < this->target_temperature) {
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
        return;
      } else {
        log_mensaje("DEBUG", "Bomba ya está apagada");
        return;
      }
    }

    if (estado_bomba_actual && sensor_temp_salida_->state < (sensor_temp_agua_->state + 1)) {
      interruptor_bomba_->turn_off();
      int64_t tiempo_total_encendido = timestamp_actual - tiempo_inicio_;
      tiempo_encendida_ += tiempo_total_encendido;
      log_mensaje("DEBUG", "Bomba apagada debido a temperatura de salida insuficiente");
      log_mensaje("WARN", "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);
      espera_ = true;
      tiempo_espera_fin_ = timestamp_actual + 300; // 5 minutos en segundos
      return;
    }

    if (estado_bomba_actual) {
      int64_t tiempo_transcurrido = timestamp_actual - tiempo_inicio_;
      log_mensaje("WARN", "Tiempo transcurrido de funcionamiento de la bomba: %02d:%02d:%02d",
               (int)(tiempo_transcurrido / 3600),
               (int)((tiempo_transcurrido % 3600) / 60),
               (int)(tiempo_transcurrido % 60));
    }

    log_mensaje("WARN", "Diferencia Sol-Agua: %.2f°C", sensor_temp_sol_->state - sensor_temp_agua_->state);
    log_mensaje("WARN", "Diferencia Salida-Agua: %.2f°C", sensor_temp_salida_->state - sensor_temp_agua_->state);
    log_mensaje("WARN", "Estado de la bomba: %d", interruptor_bomba_->state);
    log_mensaje("DEBUG", "Conteo de encendidos de la bomba: %d", conteo_encendidos_);

    this->publish_state();
  }
}

esphome::climate::ClimateTraits CustomClimate::traits() {
  auto traits = esphome::climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(true);
  traits.set_visual_min_temperature(temperatura_visual_minima_);
  traits.set_visual_max_temperature(temperatura_visual_maxima_);
  traits.set_visual_temperature_step(0.1);
  return traits;
}

void CustomClimate::control(const esphome::climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    this->mode = *call.get_mode();
  }
  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
  }
  this->publish_state();
}

}  // namespace custom_climate
