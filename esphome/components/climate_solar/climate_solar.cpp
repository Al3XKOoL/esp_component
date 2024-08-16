#include "climate_solar.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_solar {

static const char *TAG = "climate_solar";

void ClimateSolar::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ClimateSolar...");
  // Código de configuración inicial
}

void ClimateSolar::control(const climate::ClimateCall &call) {
  ESP_LOGD(TAG, "Control called");
  // Lógica para manejar comandos de control
  // Aquí podrías revisar si el usuario ha cambiado alguna configuración
}

climate::ClimateTraits ClimateSolar::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_action(true);
  traits.set_supports_two_point_target_temperature(false);
  traits.set_visual_min_temperature(this->visual_min_temp_);
  traits.set_visual_max_temperature(this->visual_max_temp_);
  traits.set_visual_temperature_step(0.1f);
  return traits;
}

void ClimateSolar::loop() {
  // Código que debe ejecutarse en cada ciclo
  update_control_logic();
}

void ClimateSolar::update_control_logic() {
  // Aquí va la lógica para actualizar el control
  // Ejemplo: Verificar si la temperatura del agua es lo suficientemente alta para encender la bomba
  // Y luego realizar las acciones correspondientes.
}

void ClimateSolar::set_temp_sun(sensor::Sensor *temp_sun) {
  this->temp_sun_ = temp_sun;
}

void ClimateSolar::set_temp_watter(sensor::Sensor *temp_watter) {
  this->temp_watter_ = temp_watter;
}

void ClimateSolar::set_temp_output(sensor::Sensor *temp_output) {
  this->temp_output_ = temp_output;
}

void ClimateSolar::set_pump_switch(switch_::Switch *pump_switch) {
  this->pump_switch_ = pump_switch;
}

void ClimateSolar::set_temp_max(float temp_max) {
  this->temp_max_ = temp_max;
}

void ClimateSolar::set_diff_high(float diff_high) {
  this->diff_high_ = diff_high;
}

void ClimateSolar::set_diff_mid(float diff_mid) {
  this->diff_mid_ = diff_mid;
}

void ClimateSolar::set_visual_min_temp(float visual_min_temp) {
  this->visual_min_temp_ = visual_min_temp;
}

void ClimateSolar::set_visual_max_temp(float visual_max_temp) {
  this->visual_max_temp_ = visual_max_temp;
}

void ClimateSolar::set_pump_power(float pump_power) {
  this->pump_power_ = pump_power;
}

}  // namespace climate_solar
}  // namespace esphome
