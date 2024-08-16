#include "climate_solar.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_solar {

static const char *TAG = "climate_solar";

void ClimateSolar::setup() {
  // Inicialización si es necesario
}

void ClimateSolar::control(const esphome::climate::ClimateCall &call) {
  if (this->pump_switch_ != nullptr && call.get_mode().has_value()) {
    if (call.get_mode().value() == esphome::climate::CLIMATE_MODE_HEAT) {
      if (this->temp_sun_ != nullptr && this->temp_watter_ != nullptr) {
        if (this->temp_sun_->state > this->temp_watter_->state + this->temp_max_) {
          this->pump_switch_->turn_on();
          ESP_LOGI(TAG, "Bomba encendida debido a la temperatura adecuada");
        } else if (this->temp_sun_->state < this->temp_watter_->state + this->temp_min_) {
          this->pump_switch_->turn_off();
          ESP_LOGI(TAG, "Bomba apagada debido a temperatura inadecuada");
        }
      }
    } else {
      this->pump_switch_->turn_off();
      ESP_LOGI(TAG, "Bomba apagada porque el modo HEAT está desactivado");
    }
  }
}

esphome::climate::ClimateTraits ClimateSolar::traits() {
  auto traits = esphome::climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(false);
  return traits;
}

void ClimateSolar::loop() {
  // Código para el loop si es necesario
}

}  // namespace climate_solar
}  // namespace esphome
