#include "custom_climate.h"
#include <cstdarg>
#include <cstdio>
#include "esphome/core/log.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace custom_climate {

static const char *TAG = "custom_climate";

void CustomClimate::setup() {
  // Inicializar valores de los números
  if (this->diferencia_media_number_ != nullptr) {
    this->diferencia_media_ = this->diferencia_media_number_->state;
  }
  if (this->diferencia_alta_number_ != nullptr) {
    this->diferencia_alta_ = this->diferencia_alta_number_->state;
  }

  // Inicializar otros componentes y variables
  this->ultimo_tiempo_verificacion_ = millis();
  this->ultimo_reset_diario_ = this->obtener_tiempo_actual();
}

void CustomClimate::loop() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - this->ultimo_tiempo_verificacion_ >= this->intervalo_segundos_ * 1000) {
    this->ultimo_tiempo_verificacion_ = tiempo_actual;

    this->control_bomba();
    this->actualizar_consumo();
    this->reset_consumo_diario();

    // Publicar estados de los sensores
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
  return traits;
}

bool CustomClimate::control_bomba() {
  float diferencia_media = this->diferencia_media_number_ != nullptr ? this->diferencia_media_number_->state : this->diferencia_media_;
  float diferencia_alta = this->diferencia_alta_number_ != nullptr ? this->diferencia_alta_number_->state : this->diferencia_alta_;

  if (this->espera_) {
    if (this->obtener_tiempo_actual() >= this->tiempo_espera_fin_) {
      this->espera_ = false;
    } else {
      return false;
    }
  }

  if (this->modo_cerca_temperatura_objetivo()) {
    this->control_bomba_cerca_objetivo();
  } else {
    this->control_bomba_normal();
  }

  return true;
}

bool CustomClimate::modo_cerca_temperatura_objetivo() {
  return std::abs(this->get_current_temperature() - this->target_temperature) <= this->temperatura_cerca_;
}

void CustomClimate::control_bomba_cerca_objetivo() {
  if (this->diferencia_temperatura_suficiente()) {
    if (this->get_current_temperature() < this->target_temperature) {
      this->encender_bomba();
    } else {
      this->apagar_bomba();
    }
  } else {
    this->apagar_bomba();
  }
}

void CustomClimate::control_bomba_normal() {
  if (this->diferencia_temperatura_suficiente()) {
    this->encender_bomba();
  } else {
    this->apagar_bomba();
  }
}

bool CustomClimate::diferencia_temperatura_suficiente() {
  float diferencia_media = this->diferencia_media_number_ != nullptr ? this->diferencia_media_number_->state : this->diferencia_media_;
  float diferencia_alta = this->diferencia_alta_number_ != nullptr ? this->diferencia_alta_number_->state : this->diferencia_alta_;

  float temp_sol = this->sensor_temp_sol_->state;
  float temp_agua = this->sensor_temp_agua_->state;
  float temp_salida = this->sensor_temp_salida_->state;

  return (temp_sol - temp_agua >= diferencia_media) || (temp_salida - temp_agua >= diferencia_alta);
}

void CustomClimate::encender_bomba() {
  if (!this->interruptor_bomba_->state) {
    this->interruptor_bomba_->turn_on();
    this->conteo_encendidos_++;
    this->tiempo_inicio_ = this->obtener_tiempo_actual();
    this->esperar_estabilizacion();
  }
}

void CustomClimate::apagar_bomba() {
  if (this->interruptor_bomba_->state) {
    this->interruptor_bomba_->turn_off();
    this->actualizar_consumo();
    this->activar_espera_proporcional();
  }
}

void CustomClimate::esperar_estabilizacion() {
  this->activar_espera_fija();
}

void CustomClimate::activar_espera_proporcional() {
  int64_t tiempo_encendido = this->obtener_tiempo_actual() - this->tiempo_inicio_;
  int64_t tiempo_espera = tiempo_encendido * this->factor_tiempo_activacion_;
  this->tiempo_espera_fin_ = this->obtener_tiempo_actual() + tiempo_espera;
  this->espera_ = true;
}

void CustomClimate::activar_espera_fija() {
  this->tiempo_espera_fin_ = this->obtener_tiempo_actual() + 60;  // 60 segundos de espera fija
  this->espera_ = true;
}

int64_t CustomClimate::obtener_tiempo_actual() {
  if (this->tiempo_sntp_ != nullptr && this->tiempo_sntp_->now().is_valid()) {
    return this->tiempo_sntp_->now().timestamp;
  } else if (this->tiempo_homeassistant_ != nullptr && this->tiempo_homeassistant_->now().is_valid()) {
    return this->tiempo_homeassistant_->now().timestamp;
  } else {
    return millis() / 1000;
  }
}

float CustomClimate::get_current_temperature() {
  return this->sensor_temp_agua_->state;
}

void CustomClimate::actualizar_consumo() {
  int64_t tiempo_actual = this->obtener_tiempo_actual();
  if (this->interruptor_bomba_->state) {
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
