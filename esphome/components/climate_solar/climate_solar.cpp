#include "climate_solar.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_solar {

static const char *const TAG = "climate_solar";

void ClimateSolar::setup() {
  this->last_cycle_times_ = {0, 0, 0};
  this->daily_active_time_ = 0;
  this->last_reset_time_ = millis();

  if (this->last_cycle_time_sensor_) {
    this->last_cycle_time_sensor_->set_name("Last Cycle Time");
    App.register_sensor(this->last_cycle_time_sensor_);
  }

  if (this->daily_active_time_sensor_) {
    this->daily_active_time_sensor_->set_name("Daily Active Time");
    App.register_sensor(this->daily_active_time_sensor_);
  }

  if (this->daily_energy_consumption_sensor_) {
    this->daily_energy_consumption_sensor_->set_name("Daily Energy Consumption");
    App.register_sensor(this->daily_energy_consumption_sensor_);
  }
}

void ClimateSolar::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    climate::ClimateMode mode = *call.get_mode();
    if (mode == climate::CLIMATE_MODE_HEAT) {
      if (!this->bomba_activa) {
        if (this->temp_sun_ && this->temp_watter_) {
          if (this->temp_watter_->state < this->temp_max_ && 
              (this->temp_sun_->state - this->temp_watter_->state) >= this->diff_high_) {
            this->activate_pump();
            this->bomba_activa = true;
            this->cycle_start_time_ = millis();
            ESP_LOGI(TAG, "Bomba encendida debido a la temperatura adecuada");
          }
        }
      } else {
        if (this->temp_output_ && this->temp_watter_) {
          if ((this->temp_output_->state - this->temp_watter_->state) < this->diff_mid_) {
            this->deactivate_pump();
            this->bomba_activa = false;
            uint32_t cycle_time = millis() - this->cycle_start_time_;
            this->last_cycle_times_.push_back(cycle_time);
            if (this->last_cycle_times_.size() > 3) {
              this->last_cycle_times_.erase(this->last_cycle_times_.begin());
            }
            this->daily_active_time_ += cycle_time;
            ESP_LOGI(TAG, "Bomba apagada debido a temperatura inadecuada");
          }
        }
      }
    } else {
      this->deactivate_pump();
      this->bomba_activa = false;
      ESP_LOGI(TAG, "Bomba apagada porque el modo HEAT está desactivado");
    }
  }
}

climate::ClimateTraits ClimateSolar::traits() {
  climate::ClimateTraits traits;
  traits.set_supports_current_temperature(true);
  traits.set_supports_action(true);
  traits.set_supported_modes({climate::CLIMATE_MODE_HEAT, climate::CLIMATE_MODE_OFF});
  traits.set_visual_min_temperature(this->visual_min_temp_);
  traits.set_visual_max_temperature(this->visual_max_temp_);
  return traits;
}

void ClimateSolar::loop() {
  if (millis() - this->last_reset_time_ >= 86400000) { // 24 horas
    this->daily_active_time_ = 0;
    this->last_reset_time_ = millis();
  }

  if (this->last_cycle_time_sensor_) {
    float avg_cycle_time = (this->last_cycle_times_.size() > 0) ?
      (this->last_cycle_times_[0] + this->last_cycle_times_[1] + this->last_cycle_times_[2]) / 1000.0 : 0;
    this->last_cycle_time_sensor_->publish_state(avg_cycle_time);
  }

  if (this->daily_active_time_sensor_) {
    this->daily_active_time_sensor_->publish_state(this->daily_active_time_ / 1000.0);
  }

  if (this->daily_energy_consumption_sensor_) {
    float energy_consumption = (this->daily_active_time_ / 3600000.0) * this->pump_power_;
    this->daily_energy_consumption_sensor_->publish_state(energy_consumption);
  }
}

void ClimateSolar::activate_pump() {
  ESP_LOGI(TAG, "Bomba activada.");
  // Aquí debes agregar el código para activar la bomba
}

void ClimateSolar::deactivate_pump() {
  ESP_LOGI(TAG, "Bomba desactivada.");
  // Aquí debes agregar el código para desactivar la bomba
}

}  // namespace climate_solar
}  // namespace esphome
