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
  void setup() override;  // Configuración inicial
  void control(const climate::ClimateCall &call) override;  // Manejar comandos de control
  climate::ClimateTraits traits() override;  // Atributos del componente de clima
  void loop() override;  // Lógica que se ejecuta en cada ciclo

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
  sensor::Sensor *temp_sun_{nullptr};  // Sensor de temperatura solar
  sensor::Sensor *temp_watter_{nullptr};  // Sensor de temperatura del agua
  sensor::Sensor *temp_output_{nullptr};  // Sensor de temperatura de salida
  switch_::Switch *pump_switch_{nullptr};  // Interruptor de la bomba

  float temp_max_{37.5f};  // Temperatura máxima
  float diff_high_{1.0f};  // Diferencial alto
  float diff_mid_{1.0f};  // Diferencial medio
  float visual_min_temp_{25.0f};  // Temperatura mínima visual
  float visual_max_temp_{40.0f};  // Temperatura máxima visual
  float pump_power_{100.0f};  // Potencia de la bomba

  void update_control_logic();  // Lógica de control
};

}  // namespace climate_solar
}  // namespace esphome

#endif  // CLIMATE_SOLAR_H
