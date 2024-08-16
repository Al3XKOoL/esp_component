#pragma once

#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace climate_solar {

class ClimateSolar : public climate::Climate, public Component {
 public:
  void setup() override;
  void loop() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void set_temp_sun(sensor::Sensor *temp_sun) { this->temp_sun_ = temp_sun; }
  void set_temp_watter(sensor::Sensor *temp_watter) { this->temp_watter_ = temp_watter; }
  void set_temp_output(sensor::Sensor *temp_output) { this->temp_output_ = temp_output; }
  void set_temp_max(float temp_max) { this->temp_max_ = temp_max; }
  void set_diff_high(float diff_high) { this->diff_high_ = diff_high; }
  void set_diff_mid(float diff_mid) { this->diff_mid_ = diff_mid; }
  void set_visual_min_temp(float visual_min_temp) { this->visual_min_temp_ = visual_min_temp; }
  void set_visual_max_temp(float visual_max_temp) { this->visual_max_temp_ = visual_max_temp; }
  void set_pump_power(float pump_power) { this->pump_power_ = pump_power; }
  void set_pump_switch(switch::Switch *pump_switch) { this->pump_switch_ = pump_switch; }

 protected:
  sensor::Sensor *temp_sun_{nullptr};
  sensor::Sensor *temp_watter_{nullptr};
  sensor::Sensor *temp_output_{nullptr};
  switch::Switch *pump_switch_{nullptr};

  float temp_max_{0};
  float diff_high_{0};
  float diff_mid_{0};
  float visual_min_temp_{0};
  float visual_max_temp_{0};
  float pump_power_{0};
  bool bomba_activa{false};
  uint32_t cycle_start_time_{0};
  uint32_t last_reset_time_{0};
  std::vector<uint32_t> last_cycle_times_;
  float daily_active_time_{0};
};

}  // namespace climate_solar
}  // namespace esphome
