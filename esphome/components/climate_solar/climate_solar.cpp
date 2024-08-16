#include "climate_solar.h"

namespace esphome {
namespace climate_solar {

void ClimateSolar::setup() {
  // Inicializar los sensores
  this->last_cycle_time_sensor_ = new esphome::sensor::Sensor();
  this->daily_active_time_sensor_ = new esphome::sensor::Sensor();
  this->daily_energy_consumption_sensor_ = new esphome::sensor::Sensor();
  App.register_sensor(this->last_cycle_time_sensor_);
  App.register_sensor(this->daily_active_time_sensor_);
  App.register_sensor(this->daily_energy_consumption_sensor_);
}

void ClimateSolar::control(const esphome::climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    esphome::climate::ClimateMode mode = *call.get_mode();
    if (mode == esphome::climate::CLIMATE_MODE_HEAT) {
      if (!this->bomba_activa) {
        if (this->temp_sun_->state < this->temp_max_ && 
            (this->temp_sun_->state - this->temp_watter_->state) >= this->diff_high_) {
          this->pump_switch_->turn_on();
          this->bomba_activa = true;
          this->cycle_start_time_ = millis();
          ESP_LOGI("main", "Bomba encendida debido a la temperatura adecuada");
        }
      } else {
        if ((this->temp_output_->state - this->temp_watter_->state) < this->diff_mid_) {
          this->pump_switch_->turn_off();
          this->bomba_activa = false;
          uint32_t cycle_time = millis() - this->cycle_start_time_;
          this->last_cycle_times_.push_back(cycle_time);
          if (this->last_cycle_times_.size() > 3) {
            this->last_cycle_times_.erase(this->last_cycle_times_.begin());
          }
          this->daily_active_time_ += cycle_time;
          ESP_LOGI("main", "Bomba apagada debido a temperatura inadecuada");
        }
      }
    } else {
      this->pump_switch_->turn_off();
      this->bomba_activa = false;
      ESP_LOGI("main", "Bomba apagada porque el modo HEAT está desactivado");
    }
  }
}

esphome::climate::ClimateTraits ClimateSolar::traits() {
  auto traits = esphome::climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_action(true);
  traits.set_supported_modes({esphome::climate::CLIMATE_MODE_HEAT, esphome::climate::CLIMATE_MODE_OFF});
  traits.set_visual_min_temperature(this->visual_min_temp_);
  traits.set_visual_max_temperature(this->visual_max_temp_);
  return traits;
}

void ClimateSolar::loop() {
  // Lógica adicional para verificar la temperatura y el estado de la bomba
  if (millis() - this->last_reset_time_ >= 86400000) { // 24 horas
    this->daily_active_time_ = 0;
    this->last_reset_time_ = millis();
  }

  // Actualizar los sensores
  this->last_cycle_time_sensor_->publish_state((this->last_cycle_times_[0] + this->last_cycle_times_[1] + this->last_cycle_times_[2]) / 1000.0);
  this->daily_active_time_sensor_->publish_state(this->daily_active_time_ / 1000.0);
  this->daily_energy_consumption_sensor_->publish_state((this->daily_active_time_ / 3600000.0) * this->pump_power_);
}

}  // namespace climate_solar
}  // namespace esphome
