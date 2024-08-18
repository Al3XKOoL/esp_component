#pragma once

#include "esphome/core/component.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/template/number/template_number.h"
#include "esphome/core/preferences.h"

namespace esphome {
namespace custom_climate {

class CustomClimate : public climate::Climate, public Component {
 public:
  void set_sensor_temp_sol(sensor::Sensor *sensor) { sensor_temp_sol_ = sensor; }
  void set_sensor_temp_agua(sensor::Sensor *sensor) { sensor_temp_agua_ = sensor; }
  void set_sensor_temp_salida(sensor::Sensor *sensor) { sensor_temp_salida_ = sensor; }
  void set_diferencia_alta(float diferencia) { diferencia_alta_ = diferencia; }
  void set_diferencia_media(float diferencia) { diferencia_media_ = diferencia; }
  void set_temperatura_visual_minima(float temp) { temperatura_visual_minima_ = temp; }
  void set_temperatura_visual_maxima(float temp) { temperatura_visual_maxima_ = temp; }
  void set_potencia_bomba(float potencia) { potencia_bomba_ = potencia; }
  void set_interruptor_bomba(switch_::Switch *interruptor) { interruptor_bomba_ = interruptor; }
  void set_tiempo_sntp(time::RealTimeClock *tiempo) { tiempo_sntp_ = tiempo; }
  void set_tiempo_homeassistant(time::RealTimeClock *tiempo) { tiempo_homeassistant_ = tiempo; }
  void set_factor_tiempo_activacion(float factor) { factor_tiempo_activacion_ = factor; }
  void set_temperatura_cerca(float temp) { temperatura_cerca_ = temp; }
  
  void set_diferencia_media_number(template_::TemplateNumber *number) { diferencia_media_number_ = number; }
  void set_diferencia_alta_number(template_::TemplateNumber *number) { diferencia_alta_number_ = number; }

  void set_conteo_encendidos_sensor(sensor::Sensor *sensor) { conteo_encendidos_sensor_ = sensor; }
  void set_tiempo_encendido_sensor(sensor::Sensor *sensor) { tiempo_encendido_sensor_ = sensor; }
  void set_kwh_hoy_sensor(sensor::Sensor *sensor) { kwh_hoy_sensor_ = sensor; }
  void set_kwh_total_sensor(sensor::Sensor *sensor) { kwh_total_sensor_ = sensor; }

  void setup() override;
  void loop() override;
  void control(const climate::ClimateCall &call) override;
  climate::ClimateTraits traits() override;

  void save_preferences();
  void load_preferences();

 protected:
  sensor::Sensor *sensor_temp_sol_{nullptr};
  sensor::Sensor *sensor_temp_agua_{nullptr};
  sensor::Sensor *sensor_temp_salida_{nullptr};
  float diferencia_alta_{0};
  float diferencia_media_{0};
  float temperatura_visual_minima_{0};
  float temperatura_visual_maxima_{0};
  float potencia_bomba_{0};
  switch_::Switch *interruptor_bomba_{nullptr};
  time::RealTimeClock *tiempo_sntp_{nullptr};
  time::RealTimeClock *tiempo_homeassistant_{nullptr};
  float factor_tiempo_activacion_{0};
  float temperatura_cerca_{0};

  template_::TemplateNumber *diferencia_media_number_{nullptr};
  template_::TemplateNumber *diferencia_alta_number_{nullptr};

  sensor::Sensor *conteo_encendidos_sensor_{nullptr};
  sensor::Sensor *tiempo_encendido_sensor_{nullptr};
  sensor::Sensor *kwh_hoy_sensor_{nullptr};
  sensor::Sensor *kwh_total_sensor_{nullptr};

  unsigned long ultimo_tiempo_verificacion_{0};
  unsigned long ultimo_tiempo_verificacion_inicial_{0};
  unsigned long ultimo_tiempo_verificacion_continua_{0};
  unsigned long tiempo_espera_{0};
  const unsigned long intervalo_verificacion_inicial_{10000};  // 10 segundos
  const unsigned long intervalo_verificacion_continua_{2000};  // 2 segundos
  const unsigned long tiempo_espera_apagado_{120000};  // 2 minutos
  const unsigned long intervalo_verificacion_target_{60000};  // 60 segundos

  bool espera_{false};
  int64_t tiempo_espera_fin_{0};
  int64_t tiempo_inicio_{0};

  int conteo_encendidos_{0};
  int64_t tiempo_encendido_{0};
  float kwh_hoy_{0.0f};
  float kwh_total_{0.0f};
  int64_t ultimo_reset_diario_{0};

  bool estabilizando_{false};
  unsigned long tiempo_estabilizacion_inicio_{0};

  bool es_primera_comprobacion_continua_{true};
  unsigned long tiempo_inicio_comprobacion_continua_{0};

  enum Estado {
    COMPROBACION_INICIAL,
    COMPROBACION_CONTINUA,
    MODO_INTERMITENTE,
    ESPERA_APAGADO,
    VERIFICACION_TARGET
  };

  Estado estado_actual_{COMPROBACION_INICIAL};

  ESPPreferenceObject target_temp_pref_;
  ESPPreferenceObject mode_pref_;

  void log_mensaje(const char* nivel, const char* formato, ...);
  void control_bomba();
  void comprobacion_inicial();
  void comprobacion_continua();
  void modo_intermitente();
  void espera_apagado();
  void verificacion_target();
  void encender_bomba();
  void apagar_bomba();
  int64_t obtener_tiempo_actual();
  float get_current_temperature();
  void actualizar_consumo();
  void reset_consumo_diario();
};

}  // namespace custom_climate
}  // namespace esphome
