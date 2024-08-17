#include "custom_climate.h"
#include <cstdarg>
#include <cstdio>
#include "esphome/core/log.h"
#include "esphome/components/climate/climate.h"

namespace custom_climate {

using namespace esphome::climate;

static const char *const TAG = "custom_climate";

void CustomClimate::log_mensaje(const char* nivel, const char* formato, ...) {
    va_list args;
    va_start(args, formato);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), formato, args);
    va_end(args);
    esphome::ESP_LOGD(TAG, "\033[1;31m%s: %s\033[0m", nivel, buffer);
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
    log_mensaje("WARN", "Ejecutando loop()");

    if (!std::isnan(sensor_temp_agua_->state)) {
      this->current_temperature = sensor_temp_agua_->state;
    }

    log_mensaje("WARN", "Temperaturas - Sol: %.2f, Agua: %.2f, Salida: %.2f", 
                sensor_temp_sol_->state, sensor_temp_agua_->state, sensor_temp_salida_->state);

    if (this->mode == CLIMATE_MODE_HEAT) {
      control_bomba();
    } else {
      if (interruptor_bomba_->state) {
        apagar_bomba();
        log_mensaje("WARN", "Bomba apagada porque el modo HEAT está desactivado");
      }
    }

    this->publish_state();
  }
}

void CustomClimate::control_bomba() {
  if (espera_) {
    if (obtener_tiempo_actual() >= tiempo_espera_fin_) {
      espera_ = false;
      log_mensaje("WARN", "Reanudando verificaciones después de espera");
    } else {
      log_mensaje("WARN", "En espera hasta %lld", tiempo_espera_fin_);
      return;
    }
  }

  if (modo_cerca_temperatura_objetivo()) {
    control_bomba_cerca_objetivo();
  } else {
    control_bomba_normal();
  }
}

bool CustomClimate::modo_cerca_temperatura_objetivo() {
  return (this->current_temperature >= (this->target_temperature - temperatura_cerca_));
}

void CustomClimate::control_bomba_cerca_objetivo() {
  if (!interruptor_bomba_->state && diferencia_temperatura_suficiente()) {
    encender_bomba();
    esperar_estabilizacion();
    activar_espera_proporcional();
  } else if (interruptor_bomba_->state) {
    if (temperatura_alcanzada()) {
      apagar_bomba();
    }
  }
}

void CustomClimate::control_bomba_normal() {
  if (!interruptor_bomba_->state && diferencia_temperatura_suficiente()) {
    encender_bomba();
    esperar_estabilizacion();
  } else if (interruptor_bomba_->state && !diferencia_temperatura_suficiente()) {
    apagar_bomba();
    activar_espera_fija();
  }
}

bool CustomClimate::diferencia_temperatura_suficiente() {
  return (sensor_temp_sol_->state > (this->current_temperature + diferencia_alta_));
}

void CustomClimate::encender_bomba() {
  interruptor_bomba_->turn_on();
  log_mensaje("WARN", "Bomba encendida");
}

void CustomClimate::apagar_bomba() {
  interruptor_bomba_->turn_off();
  log_mensaje("WARN", "Bomba apagada");
}

void CustomClimate::esperar_estabilizacion() {
  log_mensaje("WARN", "Esperando 15 segundos para estabilización");
  delay(15000);  // 15 segundos
}

void CustomClimate::activar_espera_proporcional() {
  float diferencia_temp = sensor_temp_sol_->state - this->current_temperature;
  int tiempo_activacion = static_cast<int>(diferencia_temp * factor_tiempo_activacion_);
  espera_ = true;
  tiempo_espera_fin_ = obtener_tiempo_actual() + tiempo_activacion;
  log_mensaje("WARN", "Activada espera proporcional por %d segundos", tiempo_activacion);
}

void CustomClimate::activar_espera_fija() {
  espera_ = true;
  tiempo_espera_fin_ = obtener_tiempo_actual() + 180;  // 3 minutos
  log_mensaje("WARN", "Activada espera fija por 3 minutos");
}

bool CustomClimate::temperatura_alcanzada() {
  return (this->current_temperature >= this->target_temperature);
}

int64_t CustomClimate::obtener_tiempo_actual() {
  if (tiempo_homeassistant_ != nullptr) {
    return tiempo_homeassistant_->now().timestamp;
  }
  if (tiempo_sntp_ != nullptr) {
    return tiempo_sntp_->now().timestamp;
  }
  return millis() / 1000;  // Fallback a millis si no hay fuente de tiempo real
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
        apagar_bomba();
        log_mensaje("WARN", "Bomba apagada debido a cambio a modo OFF");
      } else if (this->mode == CLIMATE_MODE_HEAT) {
        log_mensaje("WARN", "Modo HEAT activado");
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
