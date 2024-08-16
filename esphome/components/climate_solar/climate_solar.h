#pragma once

#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/core/component.h"

namespace esphome {
namespace climate_solar {

class ClimateSolar : public esphome::Component, public esphome::climate::Climate {
 public:
  void set_temp_sun(esphome::sensor::Sensor *temp_sun) { this->temp_sun_ = temp_sun; }
  void set_temp_watter(esphome::sensor::Sensor *temp_watter) { this->temp_watter_ = temp_watter; }
  void set_temp_max(float temp_max) { this->temp_max_ = temp_max; }
  void set_temp_min(float temp_min) { this->temp_min_ = temp_min; }
  void set_pump_power(float pump_power) { this->pump_power_ = pump_power; }
  void set_pump_switch(esphome::switch_::Switch *pump_switch) { this->pump_switch_ = pump_switch; }

  void setup() override;
  void control(const esphome::climate::ClimateCall &call) override;
  esphome::climate::ClimateTraits traits() override;
  void loop() override;

 private:
  esphome::sensor::Sensor *temp_sun_;
  esphome::sensor::Sensor *temp_watter_;
  float temp_max_;
  float temp_min_;
  float pump_power_;
  esphome::switch_::Switch *pump_switch_;
};

}  // namespace climate_solar
}  // namespace esphome
