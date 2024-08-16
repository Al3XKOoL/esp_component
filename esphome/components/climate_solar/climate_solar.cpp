#include "climate_solar.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_solar {

static const char *const TAG = "climate_solar";

void ClimateSolar::setup() {
  // Configuración inicial sin parámetro por defecto
  this->last_cycle_times_ = {0, 0, 0};
  this->daily_active_time_ = 0;
  this->last_reset_time_ = millis();

  // Inicializar los sensores internamente
  this->last_cycle_time_sensor_ = new esphome::sensor::Sensor();
  this->daily_active_time_sensor_ = new esphome::sensor::Sensor();
  this->daily_energy_consumption_sensor_ = new esphome::sensor::Sensor();
  App.register_sensor(this->last_cycle_time_sensor_);
  App.register_sensor(this->daily_active_time_sensor_);
  App.register_sensor(this->daily_energy_consumption_sensor_);
}

void ClimateSolar::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    climate::ClimateMode mode = *call.get_mode();
    if (mode == climate::CLIMATE_MODE_HEAT) {
      if (!this->bomba_activa) {
        if (this->temp_sun_ != nullptr && this->temp_watter_ != nullptr) {
          if (this->temp_watter_->state < this->temp_max_ && 
              (this->temp_sun_->state - this->temp_watter_->state) >= this->diff_high_) {
            this->activate_pump();
            this->bomba_activa = true;
            this->cycle_start_time_ = millis();
            ESP_LOGI(TAG, "Bomba encendida debido a la temperatura adecuada");
          }
        }
      } else {
        if (this->temp_output_ != nullptr && this->temp_watter_ != nullptr) {
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
  // Lógica adicional para verificar la temperatura y el estado de la bomba
  if (millis() - this->last_reset_time_ >= 86400000) { // 24 horas
    this->daily_active_time_ = 0;
    this->last_reset_time_ = millis();
  }

  // Actualizar los sensores
  if (this->last_cycle_time_sensor_ != nullptr) {
    float avg_cycle_time = (this->last_cycle_times_.size() > 0) ?
      (this->last_cycle_times_[0] + this->last_cycle_times_[1] + this->last_cycle_times_[2]) / 1000.0 : 0;
    this->last_cycle_time_sensor_->publish_state(avg_cycle_time);
  }

  if (this->daily_active_time_sensor_ != nullptr) {
    this->daily_active_time_sensor_->publish_state(this->daily_active_time_ / 1000.0);
  }

  if (this->daily_energy_consumption_sensor_ != nullptr) {
    float energy_consumption = (this->daily_active_time_ / 3600000.0) * this->pump_power_;
    this->daily_energy_consumption_sensor_->publish_state(energy_consumption);
  }
}

void ClimateSolar::activate_pump() {
  ESP_LOGI(TAG, "Bomba activada.");
  // Aquí debes agregar el código para activar la bomba
  // Por ejemplo, cambiar el estado de un GPIO o relé
}

void ClimateSolar::deactivate_pump() {
  ESP_LOGI(TAG, "Bomba desactivada.");
  // Aquí debes agregar el código para desactivar la bomba
  // Por ejemplo, cambiar el estado de un GPIO o relé
}

}  // namespace climate_solar
}  // namespace esphome
