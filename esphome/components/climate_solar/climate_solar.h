#pragma once

#include "esphome.h"

namespace climate_solar {

class ClimateSolar : public climate::Climate, public Component {
public:
  void setup() override;
  void loop() override;
  climate::ClimateTraits traits() override;
  void control(const climate::ClimateCall &call) override;

  void set_temp_sun(sensor::Sensor *temp_sun);
  void set_temp_spa(sensor::Sensor *temp_spa);
  void set_temp_hot(sensor::Sensor *temp_hot);
  void set_pump_switch(switch_::Switch *pump_switch);
  void set_temp_diff_on(float temp_diff_on);
  void set_max_temp(float max_temp);
  void set_temp_diff_off(float temp_diff_off);

protected:
  sensor::Sensor *temp_sun_;
  sensor::Sensor *temp_spa_;
  sensor::Sensor *temp_hot_;
  switch_::Switch *pump_switch_;
  float temp_diff_on_;
  float max_temp_;
  float temp_diff_off_;
};

}  // namespace climate_solar
