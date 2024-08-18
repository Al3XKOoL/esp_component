#include "custom_climate.h"
#include <cstdarg>
#include <cstdio>
#include "esphome/core/log.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace custom_climate {

static const char *TAG = "custom_climate";

void CustomClimate::setup() {
  this->mode = climate::CLIMATE_MODE_OFF;
  this->current_temperature = get_current_temperature();
  this->target_temperature = 37.0;

  if (this->diferencia_media_number_ != nullptr) {
    this->diferencia_media_ = this->diferencia_media_number_->state;
  }
  if (this->diferencia_alta_number_ != nullptr) {
    this->diferencia_alta_ = this->diferencia_alta_number_->state;
  }

  this->ultimo_tiempo_verificacion_ = millis();
  this->ultimo_reset_diario_ = this->obtener_tiempo_actual();
  this->tiempo_estabilizacion_ = 0;

  this->publish_state();
  this->log_mensaje("INFO", "Setup completado");
}

void CustomClimate::loop() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - this->ultimo_tiempo_verificacion_ >= this->intervalo_segundos_ * 1000) {
    this->ultimo_tiempo_verificacion_ = tiempo_actual;

    float temp_actual = get_current_temperature();
    if (!std::isnan(temp_actual)) {
      this->current_temperature = temp_actual;
    } else {
      this->log_mensaje("ERROR", "La temperatura actual es NaN, revisa el sensor de temperatura de agua.");
    }

    if (this->mode == climate::CLIMATE_MODE_HEAT) {
      this->log_mensaje("DEBUG", "Modo HEAT activo, iniciando control de bomba");
      this->control_bomba();
    } else {
      this->log_mensaje("DEBUG", "Modo no es HEAT, apagando bomba");
      this->apagar_bomba();
    }

    this->actualizar_consumo();
    this->reset_consumo_diario();

    if (this->conteo_encendidos_sensor_ != nullptr) {
      this->conteo_encendidos_sensor_->publish_state(this->conteo_encendidos_);
    }
    if (this->tiempo_encendido_sensor_ != nullptr) {
      this->tiempo_encendido_sensor_->publish_state(this->tiempo_encendido_);
    }
    if (this->kwh_hoy_sensor_ != nullptr) {
      this->kwh_hoy_sensor_->publish_state(this->kwh_hoy_);
    }
    if (this->kwh_total_sensor_ != nullptr) {
      this->kwh_total_sensor_->publish_state(this->kwh_total_);
    }

    this->publish_state();
  }

  // Manejo de la estabilización
  if (this->tiempo_estabilizacion_ > 0 && millis() - this->tiempo_estabilizacion_ >= 15000) {
    this->tiempo_estabilizacion_ = 0;
    this->log_mensaje("DEBUG", "Estabilización completada");
    this->post_estabilizacion();
  }
}

void CustomClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    this->mode = *call.get_mode();
    this->publish_state();
  }
  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
    this->publish_state();
  }
}

climate::ClimateTraits CustomClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(false);
  traits.set_visual_min_temperature(this->temperatura_visual_minima_);
  traits.set_visual_max_temperature(this->temperatura_visual_maxima_);
  traits.set_supports_action(true);
  traits.add_supported_mode(climate::CLIMATE_MODE_OFF);
  traits.add_supported_mode(climate::CLIMATE_MODE_HEAT);
  return traits;
}

bool CustomClimate::control_bomba() {
  this->log_mensaje("DEBUG", "Iniciando control de bomba");

  if (this->espera_) {
    int64_t tiempo_actual = this->obtener_tiempo_actual();
    if (tiempo_actual >= this->tiempo_espera_fin_) {
      this->log_mensaje("DEBUG", "Fin del tiempo de espera");
      this->espera_ = false;
    } else {
      this->log_mensaje("DEBUG", "En espera hasta %lld (actual: %lld)", this->tiempo_espera_fin_, tiempo_actual);
      return true;
    }
  }

  bool cerca_objetivo = this->modo_cerca_temperatura_objetivo();
  this->log_mensaje("DEBUG", "Cerca del objetivo: %s", cerca_objetivo ? "Sí" : "No");

  if (cerca_objetivo) {
    this->control_bomba_cerca_objetivo();
  } else {
    this->control_bomba_normal();
  }

  this->log_mensaje("DEBUG", "Control de bomba completado");
  return true;
}

bool CustomClimate::modo_cerca_temperatura_objetivo() {
  return (this->current_temperature >= (this->target_temperature - this->temperatura_cerca_));
}

void CustomClimate::control_bomba_cerca_objetivo() {
  this->log_mensaje("DEBUG", "Control de bomba cerca del objetivo");
  if (!this->interruptor_bomba_->state && this->diferencia_temperatura_suficiente()) {
    this->encender_bomba();
    this->iniciar_estabilizacion();
  } else if (this->interruptor_bomba_->state) {
    if (this->temperatura_alcanzada()) {
      this->apagar_bomba();
    }
  }
}

void CustomClimate::control_bomba_normal() {
  this->log_mensaje("DEBUG", "Control de bomba normal");
  if (!this->interruptor_bomba_->state && this->diferencia_temperatura_suficiente()) {
    this->encender_bomba();
    this->iniciar_estabilizacion();
  } else if (this->interruptor_bomba_->state && !this->diferencia_temperatura_suficiente()) {
    this->apagar_bomba();
    this->activar_espera_fija();
  }
}

bool CustomClimate::diferencia_temperatura_suficiente() {
  float diferencia_media = this->diferencia_media_number_ != nullptr ? this->diferencia_media_number_->state : this->diferencia_media_;
  float temp_sol = this->sensor_temp_sol_->state;
  float temp_agua = this->get_current_temperature();
  
  return (temp_sol - temp_agua >= diferencia_media) && (temp_agua < this->target_temperature);
}

void CustomClimate::encender_bomba() {
  this->log_mensaje("WARN", "Encendiendo bomba");
  if (this->interruptor_bomba_ != nullptr) {
    this->interruptor_bomba_->turn_on();
    this->conteo_encendidos_++;
    this->tiempo_inicio_ = this->obtener_tiempo_actual();
  } else {
    this->log_mensaje("ERROR", "Interruptor de bomba no configurado");
  }
}

void CustomClimate::apagar_bomba() {
  this->log_mensaje("WARN", "Apagando bomba");
  if (this->interruptor_bomba_ != nullptr) {
    this->interruptor_bomba_->turn_off();
    this->actualizar_consumo();
  } else {
    this->log_mensaje("ERROR", "Interruptor de bomba no configurado");
  }
}

void CustomClimate::iniciar_estabilizacion() {
  this->log_mensaje("DEBUG", "Iniciando estabilización por 15 segundos");
  this->tiempo_estabilizacion_ = millis();
}

void CustomClimate::post_estabilizacion() {
  if (this->modo_cerca_temperatura_objetivo()) {
    this->activar_espera_proporcional();
  }
}

void CustomClimate::activar_espera_proporcional() {
  float diferencia_temp = this->sensor_temp_sol_->state - this->current_temperature;
  int tiempo_activacion = static_cast<int>(diferencia_temp * this->factor_tiempo_activacion_);
  this->espera_ = true;
  this->tiempo_espera_fin_ = this->obtener_tiempo_actual() + tiempo_activacion;
  this->log_mensaje("DEBUG", "Activada espera proporcional por %d segundos", tiempo_activacion);
}

void CustomClimate::activar_espera_fija() {
  this->espera_ = true;
  this->tiempo_espera_fin_ = this->obtener_tiempo_actual() + 180;  // 3 minutos
  this->log_mensaje("DEBUG", "Activada espera fija por 3 minutos");
}

bool CustomClimate::temperatura_alcanzada() {
  return (this->current_temperature >= this->target_temperature);
}

int64_t CustomClimate::obtener_tiempo_actual() {
  if (this->tiempo_homeassistant_ != nullptr && this->tiempo_homeassistant_->now().is_valid()) {
    return this->tiempo_homeassistant_->now().timestamp;
  } else if (this->tiempo_sntp_ != nullptr && this->tiempo_sntp_->now().is_valid()) {
    return this->tiempo_sntp_->now().timestamp;
  } else {
    return millis() / 1000;
  }
}

float CustomClimate::get_current_temperature() {
  if (this->sensor_temp_agua_ == nullptr) {
    this->log_mensaje("ERROR", "Sensor de temperatura de agua no configurado");
    return NAN;
  }
  return this->sensor_temp_agua_->state;
}

void CustomClimate::actualizar_consumo() {
  int64_t tiempo_actual = this->obtener_tiempo_actual();
  if (this->interruptor_bomba_ != nullptr && this->interruptor_bomba_->state) {
    int64_t tiempo_encendido = tiempo_actual - this->tiempo_inicio_;
    this->tiempo_encendido_ += tiempo_encendido;
    float kwh = (this->potencia_bomba_ / 1000.0f) * (tiempo_encendido / 3600.0f);
    this->kwh_hoy_ += kwh;
    this->kwh_total_ += kwh;
  }
  this->tiempo_inicio_ = tiempo_actual;
}

void CustomClimate::reset_consumo_diario() {
  int64_t tiempo_actual = this->obtener_tiempo_actual();
  if (tiempo_actual - this->ultimo_reset_diario_ >= 24 * 60 * 60) {
    this->kwh_hoy_ = 0.0f;
    this->ultimo_reset_diario_ = tiempo_actual;
    this->log_mensaje("INFO", "Reseteo diario de consumo");
  }
}

void CustomClimate::log_mensaje(const char* nivel, const char* formato, ...) {
  va_list args;
  va_start(args, formato);
  char buffer[256];
  vsnprintf(buffer, sizeof(buffer), formato, args);
  va_end(args);

  if (strcmp(nivel, "ERROR") == 0) {
    ESP_LOGE(TAG, "%s", buffer);
  } else if (strcmp(nivel, "WARN") == 0) {
    ESP_LOGW(TAG, "%s", buffer);
  } else if (strcmp(nivel, "INFO") == 0) {
    ESP_LOGI(TAG, "%s", buffer);
  } else if (strcmp(nivel, "DEBUG") == 0) {
    ESP_LOGD(TAG, "%s", buffer);
  } else if (strcmp(nivel, "VERBOSE") == 0) {
    ESP_LOGV(TAG, "%s", buffer);
  }
}

}  // namespace custom_climate
}  // namespace esphome
