#include "ClimateSolar.h"

namespace climate_solar {

void ClimateSolar::setup() {
  // Configuración inicial si es necesario
}

void ClimateSolar::loop() {
  ESP_LOGI("main", "Iniciando la verificación del control climático...");

  // Obtener el estado actual de la bomba
  bool estado_bomba_actual = this->pump_switch_->state;

  // Verificar si la bomba debe estar encendida
  if (this->temp_sun_->state > (this->temp_spa_->state + this->temp_diff_on_) && this->temp_spa_->state < this->max_temp_) {
    if (!estado_bomba_actual) {
      this->pump_switch_->turn_on();
      id(conteo_encendidos) += 1;  // Incrementa el conteo de encendidos solo cuando el clima activa la bomba
      ESP_LOGI("main", "Bomba encendida debido a la temperatura adecuada");
    } else {
      ESP_LOGI("main", "Bomba ya está encendida");
    }
  } else {
    if (estado_bomba_actual) {
      this->pump_switch_->turn_off();
      ESP_LOGI("main", "Bomba apagada debido a temperatura inadecuada");
    } else {
      ESP_LOGI("main", "Bomba ya está apagada");
    }
    return; // Salir si la bomba ha sido apagada
  }

  // Verificar la temperatura de salida
  if (estado_bomba_actual && this->temp_hot_->state < (this->temp_spa_->state + this->temp_diff_off_)) {
    this->pump_switch_->turn_off();
    ESP_LOGI("main", "Bomba apagada debido a temperatura de salida insuficiente");
  }

  // Actualizar y guardar el tiempo de funcionamiento de la bomba
  if (estado_bomba_actual) {
    id(tiempo_encendida) += 60;  // Incrementar en 60 segundos (1 minuto)
  }
  ESP_LOGW("main", "Tiempo de funcionamiento de la bomba: %d segundos", id(tiempo_encendida));

  // Logging adicional para depuración
  ESP_LOGW("main", "Diferencia Azotea-SPA: %.2f°C", this->temp_sun_->state - this->temp_spa_->state);
  ESP_LOGW("main", "Diferencia Caliente-SPA: %.2f°C", this->temp_hot_->state - this->temp_spa_->state);
  ESP_LOGW("main", "Estado de la bomba: %d", estado_bomba_actual);

  // Mostrar el conteo de encendidos
  ESP_LOGI("main", "Conteo de encendidos de la bomba: %d", id(conteo_encendidos));
}

climate::ClimateTraits ClimateSolar::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supported_modes({climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_OFF});
  return traits;
}

void ClimateSolar::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    auto mode = *call.get_mode();
    if (mode == climate::CLIMATE_MODE_HEAT) {
      this->mode = climate::CLIMATE_MODE_HEAT;
      this->pump_switch_->turn_on();
    } else if (mode == climate::CLIMATE_MODE_OFF) {
      this->mode = climate::CLIMATE_MODE_OFF;
      this->pump_switch_->turn_off();
    }
    this->publish_state();
  }
}

void ClimateSolar::set_temp_sun(sensor::Sensor *temp_sun) {
  this->temp_sun_ = temp_sun;
}

void ClimateSolar::set_temp_spa(sensor::Sensor *temp_spa) {
  this->temp_spa_ = temp_spa;
}

void ClimateSolar::set_temp_hot(sensor::Sensor *temp_hot) {
  this->temp_hot_ = temp_hot;
}

void ClimateSolar::set_pump_switch(switch_::Switch *pump_switch) {
  this->pump_switch_ = pump_switch;
}

void ClimateSolar::set_temp_diff_on(float temp_diff_on) {
  this->temp_diff_on_ = temp_diff_on;
}

void ClimateSolar::set_max_temp(float max_temp) {
  this->max_temp_ = max_temp;
}

void ClimateSolar::set_temp_diff_off(float temp_diff_off) {
  this->temp_diff_off_ = temp_diff_off;
}

}  // namespace climate_solar
