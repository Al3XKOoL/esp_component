#include "esphome/components/climate_solar/climate_solar.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_solar {

static const char *const TAG = "climate_solar";

void ClimateSolar::setup() {
  // Inicializar sensores y otros recursos si es necesario
  if (this->pump_switch_) {
    this->pump_switch_->turn_off(); // Asegúrate de que esté apagado al inicio
  }
  // Inicializa los sensores si es necesario
}

void ClimateSolar::control(const climate::ClimateCall &call) {
  if (call.mode.has_value()) {
    // Implementa la lógica de control de acuerdo con el modo
    if (call.mode.value() == climate::CLIMATE_MODE_HEAT) {
      // Implementa la lógica para encender la bomba si es necesario
      if (this->temp_sun_ && this->temp_watter_ && this->temp_output_) {
        // Lógica de control basada en temperaturas
        if (this->temp_output_->state > this->temp_max_ + this->diff_high_) {
          if (this->pump_switch_) {
            this->pump_switch_->turn_on();
          }
        } else if (this->temp_output_->state < this->temp_max_ - this->diff_mid_) {
          if (this->pump_switch_) {
            this->pump_switch_->turn_off();
          }
        }
      }
    }
  }

  if (call.target_temperature.has_value()) {
    // Implementa la lógica para manejar la temperatura objetivo
  }
}

void ClimateSolar::loop() {
  // Implementa la lógica del ciclo principal aquí
  if (this->temp_sun_ && this->temp_watter_ && this->temp_output_) {
    // Aquí podrías actualizar los sensores, calcular tiempos y consumo, etc.
    float total_cycle_time = (this->last_cycle_times_[0] + this->last_cycle_times_[1] + this->last_cycle_times_[2]) / 1000.0;
    if (this->last_cycle_time_sensor_) {
      this->last_cycle_time_sensor_->publish_state(total_cycle_time);
    }
    if (this->daily_active_time_sensor_) {
      this->daily_active_time_sensor_->publish_state(this->daily_active_time_ / 1000.0);
    }
    if (this->daily_energy_consumption_sensor_) {
      this->daily_energy_consumption_sensor_->publish_state((this->daily_active_time_ / 3600000.0) * this->pump_power_);
    }
  }
}

}  // namespace climate_solar
}  // namespace esphome
