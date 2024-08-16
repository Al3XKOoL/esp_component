#pragma once

#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/core/component.h"

namespace esphome {
namespace climate_solar {

class ClimateSolar : public esphome::Component, public climate::Climate {
 public:
  void setup() override;
  void loop() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void set_last_cycle_time_sensor(sensor::Sensor *sensor) { last_cycle_time_sensor_ = sensor; }
  void set_daily_active_time_sensor(sensor::Sensor *sensor) { daily_active_time_sensor_ = sensor; }
  void set_daily_energy_consumption_sensor(sensor::Sensor *sensor) { daily_energy_consumption_sensor_ = sensor; }
  void set_temp_sun(sensor::Sensor *sensor) { temp_sun_ = sensor; }
  void set_temp_watter(sensor::Sensor *sensor) { temp_watter_ = sensor; }
  void set_temp_output(sensor::Sensor *sensor) { temp_output_ = sensor; }
  void set_temp_max(float temp) { temp_max_ = temp; }
  void set_temp_min(float temp) { temp_min_ = temp; }
  void set_diff_high(float diff) { diff_high_ = diff; }
  void set_diff_mid(float diff) { diff_mid_ = diff; }
  void set_visual_min_temp(float temp) { visual_min_temp_ = temp; }
  void set_visual_max_temp(float temp) { visual_max_temp_ = temp; }
  void set_pump_power(float power) { pump_power_ = power; }

 protected:
  void activate_pump();
  void deactivate_pump();

 private:
  sensor::Sensor *last_cycle_time_sensor_{nullptr};
  sensor::Sensor *daily_active_time_sensor_{nullptr};
  sensor::Sensor *daily_energy_consumption_sensor_{nullptr};
  sensor::Sensor *temp_sun_{nullptr};
  sensor::Sensor *temp_watter_{nullptr};
  sensor::Sensor *temp_output_{nullptr};

  bool bomba_activa{false};
  unsigned long cycle_start_time_{0};
  std::vector<unsigned long> last_cycle_times_;
  unsigned long daily_active_time_{0};
  unsigned long last_reset_time_{0};
  float temp_max_{35.0};
  float temp_min_{25.0};
  float diff_high_{5.0};
  float diff_mid_{2.0};
  float visual_min_temp_{15.0};
  float visual_max_temp_{40.0};
  float pump_power_{0.0};
};

}  // namespace climate_solar
}  // namespace esphome
