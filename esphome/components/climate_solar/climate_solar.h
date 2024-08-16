#include "climate_solar.h"

void ClimateSolar::setup() {
  // Initialization code here
}

void ClimateSolar::control(const esphome::climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    esphome::climate::ClimateMode mode = *call.get_mode();
    if (mode == esphome::climate::CLIMATE_MODE_HEAT) {
      if (!this->bomba_activa) {
        if (this->temp_watter_->state < this->temp_max_ &&
            (this->temp_sun_->state - this->temp_watter_->state) >= this->diff_high_) {
          this->pump_switch_->turn_on();  // Encender la bomba
          this->bomba_activa = true;
          this->cycle_start_time_ = millis();
          ESP_LOGI("main", "Bomba encendida debido a la temperatura adecuada");
        }
      } else {
        if ((this->temp_output_->state - this->temp_watter_->state) < this->diff_mid_) {
          this->pump_switch_->turn_off();  // Apagar la bomba
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
      this->pump_switch_->turn_off();  // Apagar la bomba si el modo HEAT está desactivado
      this->bomba_activa = false;
      ESP_LOGI("main", "Bomba apagada porque el modo HEAT está desactivado");
    }
  }
}
