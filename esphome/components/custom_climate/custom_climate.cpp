#include "custom_climate.h"
#include <cstdarg>
#include <cstdio>
#include "esphome/core/log.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace custom_climate {

static const char *TAG = "custom_climate";

void CustomClimate::setup() {
  // Inicializar las preferencias
  this->target_temp_pref_ = global_preferences->make_preference<float>(this->get_object_id_hash() + 1);
  this->mode_pref_ = global_preferences->make_preference<int>(this->get_object_id_hash() + 2);

  // Cargar preferencias
  this->load_preferences();

  // Establecer la temperatura actual
  this->current_temperature = get_current_temperature();

  // Solo establecer valores por defecto si no se cargaron de las preferencias
  if (std::isnan(this->target_temperature)) {
    this->target_temperature = 37.0;
  }

  if (this->mode == climate::CLIMATE_MODE_AUTO) {  // Asumiendo que AUTO no es un modo válido en tu caso
    this->mode = climate::CLIMATE_MODE_OFF;
  }

  if (this->diferencia_media_number_ != nullptr) {
    this->diferencia_media_ = this->diferencia_media_number_->state;
  }

  if (this->diferencia_alta_number_ != nullptr) {
    this->diferencia_alta_ = this->diferencia_alta_number_->state;
  }

  this->ultimo_tiempo_verificacion_ = millis();
  this->ultimo_reset_diario_ = this->obtener_tiempo_actual();

  if (this->interruptor_bomba_ != nullptr && this->interruptor_bomba_->state) {
    this->estado_actual_ = COMPROBACION_CONTINUA;
  }

  this->es_primera_comprobacion_continua_ = true;
  this->tiempo_inicio_comprobacion_continua_ = 0;

  this->publish_state();

  ESP_LOGI(TAG, "Setup completado. Modo: %d, Temperatura objetivo: %.1f", static_cast<int>(this->mode), this->target_temperature);
}

void CustomClimate::loop() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - this->ultimo_tiempo_verificacion_ >= 10000) {
    this->ultimo_tiempo_verificacion_ = tiempo_actual;

    float temp_actual = get_current_temperature();
    if (!std::isnan(temp_actual)) {
      this->current_temperature = temp_actual;
    } else {
      ESP_LOGE(TAG, "La temperatura actual es NaN, revisa el sensor de temperatura de agua.");
    }

    if (this->mode == climate::CLIMATE_MODE_HEAT) {
      this->control_bomba();
    } else {
      this->apagar_bomba();
    }

    this->actualizar_consumo();
    this->reset_consumo_diario();

    if (this->conteo_encendidos_sensor_ != nullptr) {
      this->conteo_encendidos_sensor_->publish_state(this->conteo_encendidos_);
      ESP_LOGD(TAG, "Conteo Encendidos: %d", this->conteo_encendidos_);
    }

    if (this->tiempo_encendido_sensor_ != nullptr) {
      this->tiempo_encendido_sensor_->publish_state(this->tiempo_encendido_);
      ESP_LOGD(TAG, "Tiempo Encendido: %lld s", this->tiempo_encendido_);
    }

    if (this->kwh_hoy_sensor_ != nullptr) {
      this->kwh_hoy_sensor_->publish_state(this->kwh_hoy_);
      ESP_LOGD(TAG, "kWh Hoy: %.3f kWh", this->kwh_hoy_);
    }

    if (this->kwh_total_sensor_ != nullptr) {
      this->kwh_total_sensor_->publish_state(this->kwh_total_);
      ESP_LOGD(TAG, "kWh Total: %.3f kWh", this->kwh_total_);
    }

    this->publish_state();

    ESP_LOGD(TAG, "Estado - Modo: %d, Acción: %d, Temp Actual: %.2f°C, Temp Objetivo: %.2f°C",
             this->mode, this->action, this->current_temperature, this->target_temperature);
  }
}

void CustomClimate::control_bomba() {
  ESP_LOGD(TAG, "Estado actual: %d", this->estado_actual_);

  switch (this->estado_actual_) {
    case COMPROBACION_INICIAL:
      this->comprobacion_inicial();
      break;
    case COMPROBACION_CONTINUA:
      this->comprobacion_continua();
      break;
    case MODO_INTERMITENTE:
      this->modo_intermitente();
      break;
    case ESPERA_APAGADO:
      this->espera_apagado();
      break;
    case VERIFICACION_TARGET:
      this->verificacion_target();
      break;
  }
}

void CustomClimate::comprobacion_inicial() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - this->ultimo_tiempo_verificacion_inicial_ >= this->intervalo_verificacion_inicial_) {
    this->ultimo_tiempo_verificacion_inicial_ = tiempo_actual;

    float temp_sol = this->sensor_temp_sol_->state;
    float temp_agua = this->get_current_temperature();

    ESP_LOGD(TAG, "Comprobación inicial: Temp sol: %.2f, Temp agua: %.2f, Diferencia alta: %.2f", temp_sol, temp_agua, this->diferencia_alta_);
    ESP_LOGD(TAG, "Tiempo de espera para próxima comprobación inicial: %lu ms", this->intervalo_verificacion_inicial_);

    if (temp_sol >= (temp_agua + this->diferencia_alta_) && temp_agua < this->target_temperature) {
      this->encender_bomba();
      this->estado_actual_ = COMPROBACION_CONTINUA;
      this->es_primera_comprobacion_continua_ = true;
      this->tiempo_inicio_comprobacion_continua_ = tiempo_actual;
      ESP_LOGD(TAG, "Cambiando a comprobación continua (primera vez)");
    }
  }
}

void CustomClimate::comprobacion_continua() {
  unsigned long tiempo_actual = millis();
  unsigned long intervalo_verificacion = this->es_primera_comprobacion_continua_ ? 10000 : this->intervalo_verificacion_continua_;
  
  if (tiempo_actual - this->ultimo_tiempo_verificacion_continua_ >= intervalo_verificacion) {
    this->ultimo_tiempo_verificacion_continua_ = tiempo_actual;

    if (this->es_primera_comprobacion_continua_ && (tiempo_actual - this->tiempo_inicio_comprobacion_continua_ >= 10000)) {
      this->es_primera_comprobacion_continua_ = false;
      ESP_LOGD(TAG, "Fin del periodo inicial de 10 segundos en comprobación continua");
    }

    float temp_caliente = this->sensor_temp_salida_->state;
    float temp_agua = this->get_current_temperature();

    ESP_LOGD(TAG, "Comprobación continua: Temp caliente: %.2f, Temp agua: %.2f, Diferencia media: %.2f", temp_caliente, temp_agua, this->diferencia_media_);
    ESP_LOGD(TAG, "Tiempo de espera para próxima comprobación continua: %lu ms", intervalo_verificacion);

    if (temp_caliente >= (temp_agua + this->diferencia_media_) && temp_agua < this->target_temperature) {
      if (temp_agua >= (this->target_temperature - this->temperatura_cerca_)) {
        this->estado_actual_ = MODO_INTERMITENTE;
        ESP_LOGD(TAG, "Cambiando a modo intermitente");
      }
    } else {
      this->apagar_bomba();
      this->tiempo_espera_ = tiempo_actual;
      this->estado_actual_ = ESPERA_APAGADO;
      ESP_LOGD(TAG, "Cambiando a espera apagado");
    }
  }
}

void CustomClimate::modo_intermitente() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - this->ultimo_tiempo_verificacion_continua_ >= this->intervalo_verificacion_continua_) {
    this->ultimo_tiempo_verificacion_continua_ = tiempo_actual;

    float temp_caliente = this->sensor_temp_salida_->state;
    float temp_agua = this->get_current_temperature();

    ESP_LOGD(TAG, "Modo intermitente: Temp caliente: %.2f, Temp agua: %.2f, Diferencia media: %.2f", temp_caliente, temp_agua, this->diferencia_media_);
    ESP_LOGD(TAG, "Tiempo de espera para próxima verificación intermitente: %lu ms", this->intervalo_verificacion_continua_);

    if (temp_agua >= (this->target_temperature - this->temperatura_cerca_)) {
      this->apagar_bomba();
      ESP_LOGD(TAG, "Temperatura cerca del objetivo, apagando bomba en modo intermitente");
    } else if (temp_caliente >= (temp_agua + this->diferencia_media_)) {
      if (!this->interruptor_bomba_->state) {
        this->encender_bomba();
        ESP_LOGD(TAG, "Encendiendo bomba en modo intermitente");
      }
    } else {
      this->apagar_bomba();
      ESP_LOGD(TAG, "Apagando bomba en modo intermitente");
    }

    if (temp_agua >= this->target_temperature) {
      this->apagar_bomba();
      this->estado_actual_ = VERIFICACION_TARGET;
      ESP_LOGD(TAG, "Cambiando a verificación target");
    }
  }
}

void CustomClimate::espera_apagado() {
  unsigned long tiempo_actual = millis();
  unsigned long tiempo_restante = this->tiempo_espera_apagado_ - (tiempo_actual - this->tiempo_espera_);
  ESP_LOGD(TAG, "Espera apagado: Tiempo restante: %lu ms", tiempo_restante);
  
  if (tiempo_actual - this->tiempo_espera_ >= this->tiempo_espera_apagado_) {
    this->estado_actual_ = COMPROBACION_INICIAL;
    ESP_LOGD(TAG, "Fin de espera, cambiando a comprobación inicial");
  }
}

void CustomClimate::verificacion_target() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - this->ultimo_tiempo_verificacion_continua_ >= this->intervalo_verificacion_target_) {
    this->ultimo_tiempo_verificacion_continua_ = tiempo_actual;

    float temp_agua = this->get_current_temperature();

    ESP_LOGD(TAG, "Verificación target: Temp agua: %.2f, Target: %.2f", temp_agua, this->target_temperature);
    ESP_LOGD(TAG, "Tiempo de espera para próxima verificación target: %lu ms", this->intervalo_verificacion_target_);

    if (temp_agua < this->target_temperature) {
      this->estado_actual_ = COMPROBACION_INICIAL;
      ESP_LOGD(TAG, "Temperatura por debajo del target, cambiando a comprobación inicial");
    }
  }
}

void CustomClimate::encender_bomba() {
  ESP_LOGD(TAG, "Encendiendo bomba");
  if (this->interruptor_bomba_ != nullptr) {
    this->interruptor_bomba_->turn_on();
    this->conteo_encendidos_++;
    this->tiempo_inicio_ = this->obtener_tiempo_actual();
    this->action = climate::CLIMATE_ACTION_HEATING;
    this->publish_state();
  } else {
    ESP_LOGE(TAG, "Interruptor de bomba no configurado");
  }
}

void CustomClimate::apagar_bomba() {
  ESP_LOGD(TAG, "Apagando bomba");
  if (this->interruptor_bomba_ != nullptr) {
    this->interruptor_bomba_->turn_off();
    this->actualizar_consumo();
    this->action = climate::CLIMATE_ACTION_IDLE;
    this->publish_state();
  } else {
    ESP_LOGE(TAG, "Interruptor de bomba no configurado");
  }
}

void CustomClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    this->mode = *call.get_mode();
    this->save_preferences();
    ESP_LOGD(TAG, "Modo cambiado a: %d", this->mode);
    this->publish_state();
  }
  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
    this->save_preferences();
    ESP_LOGD(TAG, "Temperatura objetivo cambiada a: %.2f", this->target_temperature);
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
    ESP_LOGE(TAG, "Sensor de temperatura de agua no configurado");
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
    ESP_LOGE(TAG, "Consumo actualizado: %.3f kWh hoy, %.3f kWh total", this->kwh_hoy_, this->kwh_total_);
  }
  this->tiempo_inicio_ = tiempo_actual;
}

void CustomClimate::reset_consumo_diario() {
  int64_t tiempo_actual = this->obtener_tiempo_actual();
  if (tiempo_actual - this->ultimo_reset_diario_ >= 24 * 60 * 60) {
    this->kwh_hoy_ = 0.0f;
    this->ultimo_reset_diario_ = tiempo_actual;
    ESP_LOGE(TAG, "Reseteo diario de consumo");
  }
}

}  // namespace custom_climate
}  // namespace esphome
