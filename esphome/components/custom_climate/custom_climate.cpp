#include "custom_climate.h"
#include "esphome/core/log.h"

namespace esphome {
namespace custom_climate {

static const char *TAG = "custom_climate";

void CustomClimate::setup() {
  ESP_LOGI(TAG, "Configurando CustomClimate...");
  // Aquí puedes agregar cualquier configuración inicial que necesites.
}

void CustomClimate::loop() {
  unsigned long current_time = millis();
  
  // Verifica si es tiempo de ejecutar la lógica de control
  if (current_time - last_check_time_ >= interval_seconds_ * 1000UL) {
    last_check_time_ = current_time;
    check_and_control_pump();  // Ejecuta la lógica de control de la bomba
    update_state();  // Actualiza y publica el estado del climatizador
  }
}

void CustomClimate::check_and_control_pump() {
  // Lógica de diferencias de temperatura entre sensores
  float diferencia_azotea_spa = this->temperature_azotea_sensor_->state - this->temperature_spa_sensor_->state;
  float diferencia_caliente_spa = this->temperature_caliente_sensor_->state - this->temperature_spa_sensor_->state;

  ESP_LOGD(TAG, "Diferencia Azotea-SPA: %.2f°C", diferencia_azotea_spa);
  ESP_LOGD(TAG, "Diferencia Caliente-SPA: %.2f°C", diferencia_caliente_spa);

  bool estado_bomba = false;

  if (diferencia_azotea_spa > this->temperature_difference_ || diferencia_caliente_spa > this->temperature_difference_) {
    estado_bomba = true;
  }

  // Controla la bomba solar
  if (this->bomba_solar_switch_ != nullptr) {
    if (estado_bomba) {
      this->bomba_solar_switch_->turn_on();
      ESP_LOGD(TAG, "La bomba solar está ENCENDIDA");
    } else {
      this->bomba_solar_switch_->turn_off();
      ESP_LOGD(TAG, "La bomba solar está APAGADA");
    }
  }
}

void CustomClimate::update_state() {
  ESP_LOGD(TAG, "Actualizando el estado del climatizador");

  if (this->temperature_spa_sensor_ != nullptr) {
    this->current_temperature = this->temperature_spa_sensor_->state;
    ESP_LOGD(TAG, "Temperatura actual del SPA: %.2f°C", this->current_temperature);
  }

  this->target_temperature = this->max_temperature_;  // Usando max_temperature_ como temperatura objetivo
  ESP_LOGD(TAG, "Temperatura objetivo: %.2f°C", this->target_temperature);

  this->mode = climate::CLIMATE_MODE_HEAT;

  if (this->bomba_solar_switch_->state) {
    this->action = climate::CLIMATE_ACTION_HEATING;
  } else {
    this->action = climate::CLIMATE_ACTION_IDLE;
  }

  this->publish_state();  // Publica el estado actualizado a Home Assistant
}

void CustomClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    this->mode = *call.get_mode();
    ESP_LOGD(TAG, "Cambiando modo a %d", this->mode);
  }

  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
    ESP_LOGD(TAG, "Cambiando la temperatura objetivo a: %.1f°C", this->target_temperature);
  }

  this->publish_state();  // Publica el estado actualizado después de aplicar los cambios
}

climate::ClimateTraits CustomClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_visual_min_temperature(15.0);
  traits.set_visual_max_temperature(40.0);
  traits.set_visual_temperature_step(0.5);
  traits.set_supports_two_point_target_temperature(false);
  traits.set_supports_action(true);
  return traits;
}

}  // namespace custom_climate
}  // namespace esphome
