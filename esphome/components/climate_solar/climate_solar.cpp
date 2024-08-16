#include "climate_solar.h"

namespace esphome {
namespace climate_solar {

void ClimateSolar::setup() {
  // Puedes inicializar componentes aquí si es necesario
}

void ClimateSolar::control(const climate::ClimateCall &call) {
  if (call.mode_.has_value()) {
    if (call.mode_.value() == climate::CLIMATE_MODE_HEAT) {
      if (this->pump_switch_) {
        this->pump_switch_->turn_on();
      }
    } else {
      if (this->pump_switch_) {
        this->pump_switch_->turn_off();
      }
    }
  }

  if (call.target_temperature_.has_value()) {
    float target_temp = call.target_temperature_.value();
    if (this->temp_output_) {
      this->temp_output_->publish_state(target_temp);
    }
  }
}

void ClimateSolar::loop() {
  // Implementa la lógica del bucle de control si es necesario
  unsigned long current_time = millis();  // Obtiene el tiempo actual en milisegundos
  
  // Actualiza los sensores
  if (this->last_cycle_time_sensor_) {
    // Ejemplo de cálculo de tiempo total del ciclo
    unsigned long total_cycle_time = current_time - this->last_cycle_times_[0];
    this->last_cycle_time_sensor_->publish_state(total_cycle_time);
    this->last_cycle_times_[0] = current_time;
  }
  
  if (this->daily_active_time_sensor_) {
    this->daily_active_time_sensor_->publish_state(this->daily_active_time_ / 1000.0);
  }
  
  if (this->daily_energy_consumption_sensor_) {
    this->daily_energy_consumption_sensor_->publish_state((this->daily_active_time_ / 3600000.0) * this->pump_power_);
  }
}

climate::ClimateTraits ClimateSolar::traits() const {
  climate::ClimateTraits traits;
  traits.set_supports_current_temperature(true);
  traits.set_supports_target_temperature(true);
  traits.set_supports_mode(true);
  traits.set_supports_action(true);
  return traits;
}

}  // namespace climate_solar
}  // namespace esphome
