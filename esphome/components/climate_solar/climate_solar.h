#pragma once

#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace climate_solar {

class ClimateSolar : public esphome::Component, public esphome::climate::Climate {
 public:
  void set_diff_high(float diff_high) { this->diff_high_ = diff_high; }
  void set_diff_mid(float diff_mid) { this->diff_mid_ = diff_mid; }
  void set_visual_min_temp(float visual_min_temp) { this->visual_min_temp_ = visual_min_temp; }
  void set_visual_max_temp(float visual_max_temp) { this->visual_max_temp_ = visual_max_temp; }
  void set_pump_power(float pump_power) { this->pump_power_ = pump_power; }
  void set_pump_switch(esphome::switch_::Switch *pump_switch) { this->pump_switch_ = pump_switch; }
  void set_temp_sun(esphome::sensor::Sensor *temp_sun) { this->temp_sun_ = temp_sun; }
  void set_temp_watter(esphome::sensor::Sensor *temp_watter) { this->temp_watter_ = temp_watter; }
  void set_temp_output(esphome::sensor::Sensor *temp_output) { this->temp_output_ = temp_output; }

  void setup() override;
  void control(const esphome::climate::ClimateCall &call) override;
  esphome::climate::ClimateTraits traits() override;
  void loop() override;

 private:
  float diff_high_;
  float diff_mid_;
  float visual_min_temp_;
  float visual_max_temp_;
  float pump_power_;
  esphome::switch_::Switch *pump_switch_;
  esphome::sensor::Sensor *temp_sun_;
  esphome::sensor::Sensor *temp_watter_;
  esphome::sensor::Sensor *temp_output_;
  std::vector<uint32_t> last_cycle_times_;
  uint32_t daily_active_time_;
  uint32_t last_reset_time_;
  esphome::sensor::Sensor *last_cycle_time_sensor_;
  esphome::sensor::Sensor *daily_active_time_sensor_;
  esphome::sensor::Sensor *daily_energy_consumption_sensor_;
};

}  // namespace climate_solar
}  // namespace esphome
