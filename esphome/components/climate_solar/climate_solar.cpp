#include "climate_solar.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_solar {

void ClimateSolar::setup() {
  this->last_cycle_times_ = {0, 0, 0};
  this->daily_active_time_ = 0;
  this->last_reset_time_ = millis();
  this->last_cycle_time_sensor_ = new esphome::sensor::Sensor();
  this->daily_active_time_sensor_ = new esphome::sensor::Sensor();
  this->daily_energy_consumption_sensor_ = new esphome::sensor::Sensor();

  esphome::App.register_sensor(this->last_cycle_time_sensor_);
  esphome::App.register_sensor(this->daily_active_time_sensor_);
  esphome::App.register_sensor(this->daily_energy_consumption_sensor_);
}

void ClimateSolar::control(const esphome::climate::ClimateCall &call) {
  if (this->pump_switch_ != nullptr && call.get_mode().has_value()) {
    if (call.get_mode().value() == esphome::climate::CLIMATE_MODE_HEAT) {
      if (this->temp_sun_->state > this->temp_watter_->state + this->diff_high_) {
        this->pump_switch_->turn_on();
        ESP_LOGI("main", "Bomba encendida debido a la temperatura adecuada");
      } else if (this->temp_sun_->state < this->temp_watter_->state + this->diff_mid_) {
        this->pump_switch_->turn_off();
        ESP_LOGI("main", "Bomba apagada debido a temperatura inadecuada");
      }
    } else {
      this->pump_switch_->turn_off();
      ESP_LOGI("main", "Bomba apagada porque el modo HEAT está desactivado");
    }
  }
}

esphome::climate::ClimateTraits ClimateSolar::traits() {
  auto traits = esphome::climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(false);
  return traits;
}

void ClimateSolar::loop() {
  if (millis() - this->last_reset_time_ >= 86400000) {  // 24 horas
    this->daily_active_time_ = 0;
    this->last_reset_time_ = millis();
  }

  this->last_cycle_time_sensor_->publish_state((this->last_cycle_times_[0] + this->last_cycle_times_[1] + this->last_cycle_times_[2]) / 1000.0);
  this->daily_active_time_sensor_->publish_state(this->daily_active_time_ / 1000.0);
  this->daily_energy_consumption_sensor_->publish_state((this->daily_active_time_ / 3600000.0) * this->pump_power_);
}

}  // namespace climate_solar
}  // namespace esphome
