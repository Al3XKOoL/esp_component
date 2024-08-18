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
  this->current_temperature = get_current_temperature();  // Inicializar la temperatura actual
  this->target_temperature = 37.0;  // Temperatura objetivo predeterminada

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

  this->publish_state();
  this->log_mensaje("INFO", "Setup completado");
}

void CustomClimate::loop() {
  unsigned long tiempo_actual = millis();
  if (tiempo_actual - this->ultimo_tiempo_verificacion_ >= this->intervalo_segundos_ * 1000) {
    this->ultimo_tiempo_verificacion_ = tiempo_actual;

    // Actualizar la temperatura actual
    float temp_actual = get_current_temperature();
    if (!std::isnan(temp_actual)) {
      this->current_temperature = temp_actual;
    } else {
      this->log_mensaje("ERROR", "La temperatura actual es NaN, revisa el sensor de temperatura de agua.");
    }

    // Continuar con el control de temperatura
    if (this->mode == climate::CLIMATE_MODE_HEAT) {
      this->log_mensaje("DEBUG", "Modo HEAT activo, iniciando control de bomba");
      this->control_bomba();
    } else {
      this->log_mensaje("DEBUG", "Modo no es HEAT, apagando bomba");
      this->apagar_bomba();
    }

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

    this->publish_state();
  }

  // Verificar si la estabilización ha terminado
  if (this->estabilizando_ && (millis() - this->tiempo_estabilizacion_inicio_ >= 15000)) {
    this->estabilizando_ = false;
    this->log_mensaje("DEBUG", "Estabilización completada");
  }
}

void CustomClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    climate::ClimateMode mode = call.get_mode().value();
    switch (mode) {
      case climate::CLIMATE_MODE_HEAT:
        // Manejar el modo de calefacción
        break;
      case climate::CLIMATE_MODE_OFF:
        // Manejar el modo de apagado
        break;
      // Manejar otros modos si es necesario
      default:
        break;
    }
  }

  if (call.get_target_temperature().has_value()) {
    this->target_temperature = call.get_target_temperature().value();
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
      int64_t tiempo_restante = this->tiempo_espera_fin_ - tiempo_actual;
      String tiempo_espera_formateado = this->formatear_tiempo_espera(tiempo_restante);
      time_t tiempo_actual_time_t = tiempo_actual;
      struct tm *tiempo_actual_tm = localtime(&tiempo_actual_time_t);
      char buffer_fecha[20];
      strftime(buffer_fecha, sizeof(buffer_fecha), "%Y-%m-%d %H:%M:%S", tiempo_actual_tm);
      this->log_mensaje("DEBUG", "En espera hasta %s (actual: %s)", tiempo_espera_formateado.c_str(), buffer_fecha);
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

  if (this->espera_) {
    // Detener la bomba durante el tiempo de espera proporcional
    if (this->interruptor_bomba_->state) {
      this->apagar_bomba();
      this->action = climate::CLIMATE_ACTION_OFF;
      this->publish_state();
    }
    return;
  }

  if (this->interruptor_bomba_->state && this->temperatura_alcanzada()) {
    this->apagar_bomba();
    this->action = climate::CLIMATE_ACTION_OFF;
    this->publish_state();
  } else if (!this->interruptor_bomba_->state && this->diferencia_temperatura_suficiente()) {
    this->encender_bomba();
    this->esperar_estabilizacion();
    this->activar_espera_proporcional();
    this->action = climate::CLIMATE_ACTION_IDLE;
    this->publish_state();
  } else if (this->interruptor_bomba_->state && !this->temperatura_alcanzada()) {
    this->action = climate::CLIMATE_ACTION_HEATING;
    this->publish_state();
  }
}

void CustomClimate::control_bomba_normal() {
  this->log_mensaje("DEBUG", "Control de bomba normal");
  if (!this->interruptor_bomba_->state && this->diferencia_temperatura_suficiente()) {
    this->encender_bomba();
    this->esperar_estabilizacion();
    this->action = climate::CLIMATE_ACTION_HEATING; // Actualizar la acción a "HEATING"
    this->publish_state();
  } else if (this->interruptor_bomba_->state && !this->diferencia_temperatura_suficiente()) {
    this->apagar_bomba();
    this->activar_espera_fija();
    this->action = climate::CLIMATE_ACTION_OFF; // Actualizar la acción a "OFF"
    this->publish_state();
  }
}

bool CustomClimate::diferencia_temperatura_suficiente() {
  float diferencia_media = this->diferencia_media_number_ != nullptr ? this->diferencia_media_number_->state : this->diferencia_media_;
  float temp_agua = this->get_current_temperature();

  return (temp_agua < this->target_temperature - diferencia_media);
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

void CustomClimate::esperar_estabilizacion() {
  this->log_mensaje("DEBUG", "Iniciando estabilización");
  this->estabilizando_ = true;
  this->tiempo_estabilizacion_inicio_ = millis();
}

void CustomClimate::activar_espera_proporcional() {
  float diferencia_temp = this->target_temperature - this->current_temperature;
  if (diferencia_temp > this->temperatura_cerca_) {
    // Si la diferencia de temperatura es mayor que temperatura_cerca, se activa el control normal
    this->control_bomba_normal();
    return;
  }

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

String CustomClimate::formatear_tiempo_espera(int64_t segundos) {
  int minutos = segundos / 60;
  int segundos_restantes = segundos % 60;
  char buffer[20];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", minutos, segundos_restantes);
  return String(buffer);
}

}  // namespace custom_climate
}  // namespace esphome
