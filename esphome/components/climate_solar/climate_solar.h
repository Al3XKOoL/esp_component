#ifndef CLIMATE_SOLAR_H
#define CLIMATE_SOLAR_H

#include "esphome/components/climate/climate.h"
#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace climate_solar {

class ClimateSolar : public climate::Climate, public Component {
 public:
  void setup() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;
  void loop() override;

  // Métodos adicionales para configurar sensores, switches y parámetros
  void set_temp_sun(sensor::Sensor *temp_sun);
  void set_temp_watter(sensor::Sensor *temp_watter);
  void set_temp_output(sensor::Sensor *temp_output);
  void set_pump_switch(switch_::Switch *pump_switch);
  void set_temp_max(float temp_max);
  void set_diff_high(float diff_high);
  void set_diff_mid(float diff_mid);
  void set_visual_min_temp(float visual_min_temp);
  void set_visual_max_temp(float visual_max_temp);
  void set_pump_power(float pump_power);

 protected:
  sensor::Sensor *temp_sun_{nullptr};
  sensor::Sensor *temp_watter_{nullptr};
  sensor::Sensor *temp_output_{nullptr};
  switch_::Switch *pump_switch_{nullptr};

  float temp_max_{37.5f};
  float diff_high_{1.0f};
  float diff_mid_{1.0f};
  float visual_min_temp_{25.0f};
  float visual_max_temp_{40.0f};
  float pump_power_{100.0f};

  // Variables para controlar el estado de la bomba y el consumo de energía
  bool pump_is_on_{false};
  unsigned long pump_start_time_{0};
  float total_energy_consumed_{0.0f};

  void update_control_logic();  // Lógica de control
};

}  // namespace climate_solar
}  // namespace esphome

#endif  // CLIMATE_SOLAR_H
