#pragma once

#include "esphome.h"
#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"

namespace esphome {
namespace custom_climate {

class CustomClimate : public Component, public climate::Climate {
 public:
  void setup() override;
  void loop() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void set_temperature_azotea_sensor(sensor::Sensor *sensor) { temperature_azotea_sensor_ = sensor; }
  void set_temperature_spa_sensor(sensor::Sensor *sensor) { temperature_spa_sensor_ = sensor; }
  void set_temperature_caliente_sensor(sensor::Sensor *sensor) { temperature_caliente_sensor_ = sensor; }
  void set_bomba_solar_switch(switch_::Switch *sw) { bomba_solar_switch_ = sw; }

  void set_max_temperature(float temp) { max_temperature_ = temp; }
  void set_temperature_difference(float diff) { temperature_difference_ = diff; }

 protected:
  sensor::Sensor *temperature_azotea_sensor_{nullptr};
  sensor::Sensor *temperature_spa_sensor_{nullptr};
  sensor::Sensor *temperature_caliente_sensor_{nullptr};
  switch_::Switch *bomba_solar_switch_{nullptr};

  float max_temperature_{37.5};
  float temperature_difference_{2.0};

  unsigned long last_check_time_{0};
  const unsigned long interval_seconds_{30};

  int64_t tiempo_inicio_{0};
  bool espera_{false};
  int64_t tiempo_espera_fin_{0};

  void check_and_control_pump();
};

}  // namespace custom_climate
}  // namespace esphome