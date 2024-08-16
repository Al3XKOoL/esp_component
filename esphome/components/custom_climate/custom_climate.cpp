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
  // Este método se ejecuta en cada ciclo del loop.
  // Aquí puedes implementar la lógica de control del climatizador.

  // Lógica para manejar la diferencia de temperatura entre varios sensores y la lógica de la bomba
  // Suponiendo que la lógica de control de la bomba depende de las diferencias de temperatura

  float diferencia_azotea_spa = this->temperature_azotea_sensor_->state - this->temperature_spa_sensor_->state;
  float diferencia_caliente_spa = this->temperature_caliente_sensor_->state - this->temperature_spa_sensor_->state;

  ESP_LOGD(TAG, "Diferencia Azotea-SPA: %.2f°C", diferencia_azotea_spa);
  ESP_LOGD(TAG, "Diferencia Caliente-SPA: %.2f°C", diferencia_caliente_spa);

  bool estado_bomba = false;

  // Ejemplo de lógica: Si la diferencia de temperatura supera un umbral, enciende la bomba
  if (diferencia_azotea_spa > this->temperature_difference_ || diferencia_caliente_spa > this->temperature_difference_) {
    estado_bomba = true;
  }

  // Controla la bomba solar basada en la lógica anterior
  if (this->bomba_solar_switch_ != nullptr) {
    if (estado_bomba) {
      this->bomba_solar_switch_->turn_on();
      ESP_LOGD(TAG, "La bomba solar está ENCENDIDA");
    } else {
      this->bomba_solar_switch_->turn_off();
      ESP_LOGD(TAG, "La bomba solar está APAGADA");
    }
  }

  // Actualiza y publica el estado del climatizador
  this->update_state();
}

void CustomClimate::update_state() {
  ESP_LOGD(TAG, "Actualizando el estado del climatizador");

  // Obtener la temperatura actual de SPA
  if (this->temperature_spa_sensor_ != nullptr) {
    this->current_temperature = this->temperature_spa_sensor_->state;
    ESP_LOGD(TAG, "Temperatura actual del SPA: %.2f°C", this->current_temperature);
  }

  // Aquí también puedes obtener la temperatura objetivo o cualquier otra lógica
  this->target_temperature = this->max_temperature_;  // Usando max_temperature_ como temperatura objetivo por ahora
  ESP_LOGD(TAG, "Temperatura objetivo: %.2f°C", this->target_temperature);

  // Define el modo del climatizador, por ejemplo, calefacción (HEAT)
  this->mode = climate::CLIMATE_MODE_HEAT;

  // Define la acción actual del climatizador en función de si la bomba está encendida o apagada
  if (this->bomba_solar_switch_->state) {
    this->action = climate::CLIMATE_ACTION_HEATING;
  } else {
    this->action = climate::CLIMATE_ACTION_IDLE;
  }

  // Publica el estado actualizado a Home Assistant
  this->publish_state();
}

void CustomClimate::control(const climate::ClimateCall &call) {
  // Este método maneja las solicitudes de control desde Home Assistant.

  if (call.get_mode().has_value()) {
    // Si Home Assistant está solicitando un cambio de modo (calefacción, enfriamiento, etc).
    this->mode = *call.get_mode();
    ESP_LOGD(TAG, "Cambiando modo a %d", this->mode);
  }

  if (call.get_target_temperature().has_value()) {
    // Si Home Assistant está solicitando un cambio en la temperatura objetivo.
    this->target_temperature = *call.get_target_temperature();
    ESP_LOGD(TAG, "Cambiando la temperatura objetivo a: %.1f°C", this->target_temperature);
  }

  // Aquí puedes agregar lógica para responder a los cambios de la UI de Home Assistant, si fuera necesario.

  // Publica el estado actualizado después de aplicar los cambios
  this->publish_state();
}

climate::ClimateTraits CustomClimate::traits() {
  // Define las características (traits) del climatizador que Home Assistant debe conocer.
  auto traits = climate::ClimateTraits();

  traits.set_supports_current_temperature(true);  // Soporte para mostrar la temperatura actual
  traits.set_visual_min_temperature(15.0);        // Temperatura mínima visible en la GUI
  traits.set_visual_max_temperature(40.0);        // Temperatura máxima visible en la GUI
  traits.set_visual_temperature_step(0.5);        // Incrementos de temperatura
  traits.set_supports_two_point_target_temperature(false);  // Control de un solo punto (no calefacción/enfriamiento simultáneos)
  traits.set_supports_action(true);               // Indica que soporta acciones como calentar/enfriar

  return traits;
}

}  // namespace custom_climate
}  // namespace esphome
