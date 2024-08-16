#ifndef CLIMATE_SOLAR_H
#define CLIMATE_SOLAR_H

#include "esphome.h"

class ClimateSolar : public esphome::Component, public esphome::climate::Climate {
 public:
  void setup() override;
  void control(const esphome::climate::ClimateCall &call) override;
  esphome::climate::ClimateTraits traits() override;
  void loop() override;

  // Métodos para configurar los parámetros desde la interfaz
  void set_temp_max(float temp_max) { this->temp_max_ = temp_max; }
  void set_diff_high(float diff_high) { this->diff_high_ = diff_high; }
  void set_diff_mid(float diff_mid) { this->diff_mid_ = diff_mid; }
  void set_visual_min_temp(float visual_min_temp) { this->visual_min_temp_ = visual_min_temp; }
  void set_visual_max_temp(float visual_max_temp) { this->visual_max_temp_ = visual_max_temp; }
  void set_pump_power(float pump_power) { this->pump_power_ = pump_power; }

  // Métodos para configurar los parámetros desde el YAML
  void set_parameters(esphome::sensor::Sensor *temp_sun, esphome::sensor::Sensor *temp_watter, esphome::sensor::Sensor *temp_output, float temp_max, float diff_high, float diff_mid, float visual_min_temp, float visual_max_temp, float pump_power);

 private:
  esphome::sensor::Sensor *temp_sun_;
  esphome::sensor::Sensor *temp_watter_;
  esphome::sensor::Sensor *temp_output_;
  float temp_max_;
  float diff_high_;
  float diff_mid_;
  float visual_min_temp_;
  float visual_max_temp_;
  float pump_power_;
  bool bomba_activa = false;
  uint32_t cycle_start_time_;
  std::vector<uint32_t> last_cycle_times_;
  uint32_t daily_active_time_;
  uint32_t last_reset_time_;
  esphome::sensor::Sensor *last_cycle_time_sensor_;
  esphome::sensor::Sensor *daily_active_time_sensor_;
  esphome::sensor::Sensor *daily_energy_consumption_sensor_;
};

#endif // CLIMATE_SOLAR_H
