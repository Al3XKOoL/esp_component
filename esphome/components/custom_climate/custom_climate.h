#pragma once

#include "esphome.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/time/real_time_clock.h"

namespace custom_climate {

class CustomClimate : public esphome::climate::Climate, public esphome::Component {
 public:
  void set_sensor_temp_sol(esphome::sensor::Sensor *sensor_temp_sol) { sensor_temp_sol_ = sensor_temp_sol; }
  void set_sensor_temp_agua(esphome::sensor::Sensor *sensor_temp_agua) { sensor_temp_agua_ = sensor_temp_agua; }
  void set_sensor_temp_salida(esphome::sensor::Sensor *sensor_temp_salida) { sensor_temp_salida_ = sensor_temp_salida; }
  void set_diferencia_alta(float diferencia_alta) { diferencia_alta_ = diferencia_alta; }
  void set_diferencia_media(float diferencia_media) { diferencia_media_ = diferencia_media; }
  void set_temperatura_visual_minima(float temperatura_visual_minima) { temperatura_visual_minima_ = temperatura_visual_minima; }
  void set_temperatura_visual_maxima(float temperatura_visual_maxima) { temperatura_visual_maxima_ = temperatura_visual_maxima; }
  void set_potencia_bomba(float potencia_bomba) { potencia_bomba_ = potencia_bomba; }
  void set_interruptor_bomba(esphome::switch_::Switch *interruptor_bomba) { interruptor_bomba_ = interruptor_bomba; }
  void set_tiempo_sntp(esphome::time::RealTimeClock *tiempo_sntp) { tiempo_sntp_ = tiempo_sntp; }
  void set_tiempo_homeassistant(esphome::time::RealTimeClock *tiempo_homeassistant) { tiempo_homeassistant_ = tiempo_homeassistant; }
  void set_factor_tiempo_activacion(float factor_tiempo_activacion) { factor_tiempo_activacion_ = factor_tiempo_activacion; }
  void set_temperatura_cerca(float temperatura_cerca) { temperatura_cerca_ = temperatura_cerca; }
  void set_restore_state(bool restore_state) { restore_state_ = restore_state; }

  void setup() override;
  void loop() override;
  esphome::climate::ClimateTraits traits() override;
  void control(const esphome::climate::ClimateCall &call) override;
  float get_current_temperature();
  void dump_config() override;

 protected:
  esphome::sensor::Sensor *sensor_temp_sol_;
  esphome::sensor::Sensor *sensor_temp_agua_;
  esphome::sensor::Sensor *sensor_temp_salida_;
  float diferencia_alta_;
  float diferencia_media_;
  float temperatura_visual_minima_;
  float temperatura_visual_maxima_;
  float potencia_bomba_;
  esphome::switch_::Switch *interruptor_bomba_;
  esphome::time::RealTimeClock *tiempo_sntp_{nullptr};
  esphome::time::RealTimeClock *tiempo_homeassistant_{nullptr};
  float factor_tiempo_activacion_{10.0};
  float temperatura_cerca_{1.0};
  bool restore_state_{false};

  unsigned long ultimo_tiempo_verificacion_{0};
  const unsigned long intervalo_segundos_{2};
  bool espera_{false};
  int64_t tiempo_espera_fin_{0};
  int64_t tiempo_inicio_{0};

  void log_mensaje(const char* nivel, const char* formato, ...);
};

}  // namespace custom_climate
