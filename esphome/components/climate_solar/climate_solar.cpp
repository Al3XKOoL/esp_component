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
  // Ejecutar la lógica de control en cada ciclo
  update_control_logic();
}

void ClimateSolar::update_control_logic() {
  if (temp_sun_ == nullptr || temp_watter_ == nullptr || pump_switch_ == nullptr)
    return;  // Asegurarse de que los sensores y la bomba estén configurados

  float temp_sun_value = this->temp_sun_->state;
  float temp_watter_value = this->temp_watter_->state;

  if (temp_sun_value > temp_watter_value + diff_high_ && temp_watter_value < temp_max_) {
    if (!pump_is_on_) {
      pump_switch_->turn_on();
      pump_is_on_ = true;
      pump_start_time_ = millis();  // Guardar el tiempo en que se encendió la bomba
      ESP_LOGD(TAG, "Bomba encendida");
    }
  } else {
    if (pump_is_on_) {
      pump_switch_->turn_off();
      pump_is_on_ = false;
      unsigned long pump_end_time = millis();
      unsigned long pump_duration = pump_end_time - pump_start_time_;  // Duración en milisegundos

      // Calcular energía consumida (vatios-hora)
      float duration_hours = pump_duration / 3600000.0;  // Convertir milisegundos a horas
      float energy_used = pump_power_ * duration_hours;  // Potencia en vatios multiplicada por tiempo en horas
      total_energy_consumed_ += energy_used;

      ESP_LOGD(TAG, "Bomba apagada, energía consumida: %f Wh", energy_used);
    }
  }
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
