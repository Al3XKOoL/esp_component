#pragma once

#include "esphome.h"

namespace climate_solar {

class ClimateSolar : public esphome::Component, public esphome::climate::Climate {
 public:
  // Métodos necesarios para la configuración del componente
  void setup() override;
  void loop() override;

  // Métodos setters para las variables del YAML
  void set_temp_sun(sensor::Sensor *temp_sun);
  void set_temp_watter(sensor::Sensor *temp_watter);
  void set_temp_output(sensor::Sensor *temp_output);
  void set_temp_max(float temp_max);
  void set_diff_high(float diff_high);
  void set_diff_mid(float diff_mid);
  void set_visual_min_temp(float visual_min_temp);
  void set_visual_max_temp(float visual_max_temp);
  void set_pump_power(float pump_power);
  void set_pump_switch(esphome::switch_::Switch *pump_switch);

 private:
  sensor::Sensor *temp_sun_;
  sensor::Sensor *temp_watter_;
  sensor::Sensor *temp_output_;
  esphome::switch_::Switch *pump_switch_;
  float temp_max_;
  float diff_high_;
  float diff_mid_;
  float visual_min_temp_;
  float visual_max_temp_;
  float pump_power_;
};

}  // namespace climate_solar
