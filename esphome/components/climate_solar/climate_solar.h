#pragma once

#include "esphome.h"

class ClimateSolar : public esphome::Component, public esphome::climate::Climate {
 public:
  void set_temp_sun(esphome::sensor::Sensor *temp_sun) { this->temp_sun_ = temp_sun; }
  void set_temp_watter(esphome::sensor::Sensor *temp_watter) { this->temp_watter_ = temp_watter; }
  void set_temp_output(esphome::sensor::Sensor *temp_output) { this->temp_output_ = temp_output; }
  void set_pump_switch(esphome::switch_::Switch *pump_switch) { this->pump_switch_ = pump_switch; }

  void setup() override;
  void control(const esphome::climate::ClimateCall &call) override;

 protected:
  esphome::sensor::Sensor *temp_sun_;
  esphome::sensor::Sensor *temp_watter_;
  esphome::sensor::Sensor *temp_output_;
  esphome::switch_::Switch *pump_switch_;  // Switch for the pump

  // Additional variables
  float temp_max_;
  float diff_high_;
  float diff_mid_;
  float visual_min_temp_;
  float visual_max_temp_;
  bool bomba_activa = false;
  uint32_t cycle_start_time_ = 0;
  std::vector<uint32_t> last_cycle_times_;
  uint32_t daily_active_time_ = 0;
};
