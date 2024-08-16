#include "esphome.h"
#include "climate_solar.h"

esphome::climate_solar::ClimateSolar *control_bomba;

void setup() {
  // Inicializa los sensores y switches aquí
  auto *temp_sun_sensor = new esphome::sensor::Sensor();
  auto *temp_watter_sensor = new esphome::sensor::Sensor();
  auto *temp_output_sensor = new esphome::sensor::Sensor();
  auto *pump_switch = new esphome::switch_::Switch();
  auto *last_cycle_time_sensor = new esphome::sensor::Sensor();
  auto *daily_active_time_sensor = new esphome::sensor::Sensor();
  auto *daily_energy_consumption_sensor = new esphome::sensor::Sensor();

  control_bomba = new esphome::climate_solar::ClimateSolar();
  control_bomba->set_temp_sun(temp_sun_sensor);
  control_bomba->set_temp_watter(temp_watter_sensor);
  control_bomba->set_temp_output(temp_output_sensor);
  control_bomba->set_last_cycle_time_sensor(last_cycle_time_sensor);
  control_bomba->set_daily_active_time_sensor(daily_active_time_sensor);
  control_bomba->set_daily_energy_consumption_sensor(daily_energy_consumption_sensor);
  control_bomba->set_temp_max(37.5);
  control_bomba->set_diff_high(1.0);
  control_bomba->set_diff_mid(1.0);
  control_bomba->set_visual_min_temp(25.0);
  control_bomba->set_visual_max_temp(40.0);
  control_bomba->set_pump_power(100.0);
  control_bomba->set_pump_switch(pump_switch);
}

void loop() {
  // Implementa lógica adicional en el bucle si es necesario
}
