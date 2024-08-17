#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/real_time_clock/real_time_clock.h"
#include "esphome/components/preferences/preferences.h"

namespace custom_climate {

class CustomClimate : public esphome::climate::Climate, public esphome::Component {
 public:
  void setup() override;
  void loop() override;
  esphome::climate::ClimateTraits traits() override;
  void control(const esphome::climate::ClimateCall &call) override;
  void dump_config() override;

  void restore_state_from_flash();
  void save_state_to_flash();

 protected:
  float get_current_temperature();

 private:
  void log_mensaje(const char* nivel, const char* formato, ...);

  // Variables de estado
  float target_temperature = 37.0;
  esphome::climate::ClimateMode mode = esphome::climate::CLIMATE_MODE_OFF;
  float current_temperature = NAN;
  
  // Variables adicionales
  float diferencia_alta_ = 5.0;
  float diferencia_media_ = 2.0;
  float temperatura_visual_minima_ = 10.0;
  float temperatura_visual_maxima_ = 40.0;
  float potencia_bomba_ = 1.0;
  float factor_tiempo_activacion_ = 1.0;
  float temperatura_cerca_ = 2.0;
  float conteo_encendidos_ = 0;  // Variable para contar encendidos
  int64_t tiempo_encendida_ = 0;  // Variable para tiempo total encendido
  unsigned long tiempo_inicio_ = 0;
  unsigned long ultimo_tiempo_verificacion_ = 0;
  bool espera_ = false;
  unsigned long tiempo_espera_fin_ = 0;

  // Punteros a sensores e interruptores
  esphome::sensor::Sensor *sensor_temp_sol_ = nullptr;
  esphome::sensor::Sensor *sensor_temp_agua_ = nullptr;
  esphome::sensor::Sensor *sensor_temp_salida_ = nullptr;
  esphome::switch_::Switch *interruptor_bomba_ = nullptr;
  esphome::real_time_clock::RealTimeClock *tiempo_sntp_ = nullptr;
  esphome::real_time_clock::RealTimeClock *tiempo_homeassistant_ = nullptr;
};

}  // namespace custom_climate
