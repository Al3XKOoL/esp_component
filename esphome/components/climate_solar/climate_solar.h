#pragma once

#include "esphome.h"

class ClimateSolar : public Component {
 public:
  // Setters para recibir las configuraciones desde el archivo YAML
  void set_temp_sun(Sensor *temp_sun) { this->temp_sun_ = temp_sun; }
  void set_temp_spa(Sensor *temp_spa) { this->temp_spa_ = temp_spa; }
  void set_temp_hot(Sensor *temp_hot) { this->temp_hot_ = temp_hot; }
  void set_pump_switch(Switch *pump_switch) { this->pump_switch_ = pump_switch; }
  void set_temp_diff_on(float temp_diff_on) { this->temp_diff_on_ = temp_diff_on; }
  void set_temp_diff_off(float temp_diff_off) { this->temp_diff_off_ = temp_diff_off; }
  void set_max_temp(float max_temp) { this->max_temp_ = max_temp; }
  void set_visual_min_temp(float visual_min_temp) { this->visual_min_temp_ = visual_min_temp; }
  void set_visual_max_temp(float visual_max_temp) { this->visual_max_temp_ = visual_max_temp; }
  void set_pump_power(float pump_power) { this->pump_power_ = pump_power; }

  // Método para inicializar los valores
  void setup() override {
    // Aquí puedes inicializar valores si es necesario
  }

  // Método principal que se ejecuta en cada ciclo del loop
  void loop() override {
    // Comprobación de las temperaturas medidas
    if (this->temp_sun_->has_state() && this->temp_spa_->has_state() && this->temp_hot_->has_state()) {
      float temp_sun = this->temp_sun_->state;
      float temp_spa = this->temp_spa_->state;
      float temp_hot = this->temp_hot_->state;

      // Lógica para encender/apagar la bomba
      if (temp_sun - temp_spa > this->temp_diff_on_ && temp_hot < this->max_temp_) {
        this->pump_switch_->turn_on();
      } else if (temp_sun - temp_spa < this->temp_diff_off_ || temp_hot >= this->max_temp_) {
        this->pump_switch_->turn_off();
      }
    }
  }

 private:
  // Propiedades que almacenan las referencias a los sensores y switches
  Sensor *temp_sun_;
  Sensor *temp_spa_;
  Sensor *temp_hot_;
  Switch *pump_switch_;
  float temp_diff_on_;
  float temp_diff_off_;
  float max_temp_;
  float visual_min_temp_;
  float visual_max_temp_;
  float pump_power_;
};
