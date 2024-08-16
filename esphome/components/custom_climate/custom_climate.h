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
  // Sensores de temperatura
  sensor::Sensor *temperature_azotea_sensor_{nullptr};
  sensor::Sensor *temperature_spa_sensor_{nullptr};
  sensor::Sensor *temperature_caliente_sensor_{nullptr};

  // Switch para controlar la bomba solar
  switch_::Switch *bomba_solar_switch_{nullptr};

  // Configuraciones adicionales
  float max_temperature_{37.5};             // Temperatura máxima por defecto
  float temperature_difference_{2.0};       // Diferencia de temperatura para controlar la bomba

  unsigned long last_check_time_{0};        // Tiempo de la última verificación
  const unsigned long interval_seconds_{30}; // Intervalo de verificación en segundos

  int64_t tiempo_inicio_{0};
  bool espera_{false};
  int64_t tiempo_espera_fin_{0};

  // Método que se encarga de la lógica de control de la bomba
  void check_and_control_pump();

  // Método para actualizar y publicar el estado del climatizador
  void update_state();
};

}  // namespace custom_climate
}  // namespace esphome
