#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace climate_solar {

class ClimateSolar : public climate::Climate, public Component {
 public:
  void set_temp_sun(sensor::Sensor *temp_sun) { this->temp_sun_ = temp_sun; }
  void set_temp_watter(sensor::Sensor *temp_watter) { this->temp_watter_ = temp_watter; }
  void set_temp_output(sensor::Sensor *temp_output) { this->temp_output_ = temp_output; }
  void set_last_cycle_time_sensor(sensor::Sensor *sensor) { this->last_cycle_time_sensor_ = sensor; }
  void set_daily_active_time_sensor(sensor::Sensor *sensor) { this->daily_active_time_sensor_ = sensor; }
  void set_daily_energy_consumption_sensor(sensor::Sensor *sensor) { this->daily_energy_consumption_sensor_ = sensor; }
  void set_temp_max(float temp_max) { this->temp_max_ = temp_max; }
  void set_diff_high(float diff_high) { this->diff_high_ = diff_high; }
  void set_diff_mid(float diff_mid) { this->diff_mid_ = diff_mid; }
  void set_visual_min_temp(float visual_min_temp) { this->visual_min_temp_ = visual_min_temp; }
  void set_visual_max_temp(float visual_max_temp) { this->visual_max_temp_ = visual_max_temp; }
  void set_pump_power(float pump_power) { this->pump_power_ = pump_power; }
  void set_pump_switch(switch_::Switch *pump_switch) { this->pump_switch_ = pump_switch; }

 protected:
  virtual void setup() override;
  virtual void control(const climate::ClimateCall &call) override;
  virtual void loop() override;
  
  // Implementar el método virtual puro
  virtual climate::ClimateTraits traits() const override {
    climate::ClimateTraits traits;
    traits.set_supports_current_temperature(true);
    traits.set_supports_target_temperature(true);
    traits.set_supports_mode(true);
    traits.set_supports_action(true);
    return traits;
  }

 private:
  sensor::Sensor *temp_sun_{nullptr};
  sensor::Sensor *temp_watter_{nullptr};
  sensor::Sensor *temp_output_{nullptr};
  sensor::Sensor *last_cycle_time_sensor_{nullptr};
  sensor::Sensor *daily_active_time_sensor_{nullptr};
  sensor::Sensor *daily_energy_consumption_sensor_{nullptr};
  float temp_max_{37.5};
  float diff_high_{1.0};
  float diff_mid_{1.0};
  float visual_min_temp_{25.0};
  float visual_max_temp_{40.0};
  float pump_power_{100.0};
  switch_::Switch *pump_switch_{nullptr};
  int last_cycle_times_[3]{0, 0, 0};
  unsigned long daily_active_time_{0};
};

}  // namespace climate_solar
}  // namespace esphome
