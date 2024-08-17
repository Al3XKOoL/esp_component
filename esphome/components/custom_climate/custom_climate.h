#pragma once

#include "esphome.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"

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

  void setup() override;
  void loop() override;
  esphome::climate::ClimateTraits traits() override;
  void control(const esphome::climate::ClimateCall &call) override;

  esphome::number::Number *get_diferencia_media_number() { return &diferencia_media_number_; }
  esphome::number::Number *get_diferencia_alta_number() { return &diferencia_alta_number_; }
  esphome::sensor::Sensor *get_conteo_encendidos_sensor() { return &conteo_encendidos_sensor_; }
  esphome::sensor::Sensor *get_tiempo_encendido_sensor() { return &tiempo_encendido_sensor_; }
  esphome::sensor::Sensor *get_kwh_hoy_sensor() { return &kwh_hoy_sensor_; }
  esphome::sensor::Sensor *get_kwh_total_sensor() { return &kwh_total_sensor_; }

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

  esphome::number::Number diferencia_media_number_;
  esphome::number::Number diferencia_alta_number_;
  esphome::sensor::Sensor conteo_encendidos_sensor_{"Conteo Encendidos"};
  esphome::sensor::Sensor tiempo_encendido_sensor_{"Tiempo Encendido"};
  esphome::sensor::Sensor kwh_hoy_sensor_{"kWh Hoy"};
  esphome::sensor::Sensor kwh_total_sensor_{"kWh Total"};

  unsigned long ultimo_tiempo_verificacion_{0};
  const unsigned long intervalo_segundos_{2};
  bool espera_{false};
  int64_t tiempo_espera_fin_{0};
  int64_t tiempo_inicio_{0};

  int conteo_encendidos_{0};
  int64_t tiempo_encendido_{0};
  float kwh_hoy_{0.0f};
  float kwh_total_{0.0f};
  int64_t ultimo_reset_diario_{0};

  void log_mensaje(const char* nivel, const char* formato, ...);
  bool control_bomba();
  bool modo_cerca_temperatura_objetivo();
  void control_bomba_cerca_objetivo();
  void control_bomba_normal();
  bool diferencia_temperatura_suficiente();
  void encender_bomba();
  void apagar_bomba();
  void esperar_estabilizacion();
  void activar_espera_proporcional();
  void activar_espera_fija();
  bool temperatura_alcanzada();
  int64_t obtener_tiempo_actual();
  float get_current_temperature();
  void actualizar_consumo();
  void reset_consumo_diario();
};

}  // namespace custom_climate
