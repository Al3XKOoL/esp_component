#pragma once

#include "esphome.h"
#include "esphome/components/climate/climate.h"

namespace custom_climate {

class CustomClimate : public climate::Climate, public Component {
 public:
  void set_temp_sun(sensor::Sensor *temp_sun) { temp_sun_ = temp_sun; }
  void set_temp_water(sensor::Sensor *temp_water) { temp_water_ = temp_water; }
  void set_temp_output(sensor::Sensor *temp_output) { temp_output_ = temp_output; }
  void set_temp_max(float temp_max) { temp_max_ = temp_max; }
  void set_diff_high(float diff_high) { diff_high_ = diff_high; }
  void set_diff_mid(float diff_mid) { diff_mid_ = diff_mid; }
  void set_visual_min_temp(float visual_min_temp) { visual_min_temp_ = visual_min_temp; }
  void set_visual_max_temp(float visual_max_temp) { visual_max_temp_ = visual_max_temp; }
  void set_pump_power(float pump_power) { pump_power_ = pump_power; }
  void set_pump_switch(switch_::Switch *pump_switch) { pump_switch_ = pump_switch; }

  void setup() override;
  void loop() override;
  climate::ClimateTraits traits() override;
  void control(const climate::ClimateCall &call) override;

 protected:
  sensor::Sensor *temp_sun_;
  sensor::Sensor *temp_water_;
  sensor::Sensor *temp_output_;
  float temp_max_;
  float diff_high_;
  float diff_mid_;
  float visual_min_temp_;
  float visual_max_temp_;
  float pump_power_;
  switch_::Switch *pump_switch_;

  unsigned long last_check_time_{0};
  const unsigned long interval_seconds_{30};
  bool espera_{false};
  int64_t tiempo_espera_fin_{0};
  int64_t tiempo_inicio_{0};
  int conteo_encendidos_{0};
  int64_t tiempo_encendida_{0};
};

}  // namespace custom_climate
