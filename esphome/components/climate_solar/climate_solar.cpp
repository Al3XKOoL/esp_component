#include "climate_solar.h"
#include "esphome/core/log.h"

namespace esphome {
namespace climate_solar {

static const char *TAG = "climate_solar";

void ClimateSolar::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Climate Solar...");

  // Aquí puedes inicializar cualquier cosa necesaria
  // Por ejemplo, puedes leer los valores de los sensores inmediatamente
}

void ClimateSolar::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    // Solamente estamos soportando modos OFF y HEAT
    ClimateMode mode = *call.get_mode();
    this->mode = mode;

    if (mode == climate::CLIMATE_MODE_OFF) {
      ESP_LOGD(TAG, "Climate set to OFF");
      if (this->pump_switch_ != nullptr) {
        this->pump_switch_->turn_off();
      }
      this->publish_state();
    } else if (mode == climate::CLIMATE_MODE_HEAT) {
      ESP_LOGD(TAG, "Climate set to HEAT");
      this->update_control_logic();
    }
  }
}

void ClimateSolar::update_control_logic() {
  // Verificamos si tenemos todos los sensores disponibles
  if (this->temp_sun_ == nullptr || this->temp_watter_ == nullptr || this->temp_output_ == nullptr) {
    ESP_LOGE(TAG, "Sensors are not set up properly.");
    return;
  }

  // Leemos las temperaturas actuales
  float temp_sun = this->temp_sun_->state;
  float temp_watter = this->temp_watter_->state;
  float temp_output = this->temp_output_->state;

  // Lógica para controlar la bomba
  if (temp_sun > this->temp_max_) {
    ESP_LOGD(TAG, "Sun temperature (%.2f°C) is above temp_max (%.2f°C).", temp_sun, this->temp_max_);
    if (this->pump_switch_ != nullptr) {
      this->pump_switch_->turn_off();
    }
  } else if (temp_sun - temp_watter > this->diff_high_) {
    ESP_LOGD(TAG, "Temperature difference (%.2f°C) is above diff_high (%.2f°C).", temp_sun - temp_watter, this->diff_high_);
    if (this->pump_switch_ != nullptr) {
      this->pump_switch_->turn_on();
    }
  } else if (temp_sun - temp_watter > this->diff_mid_) {
    ESP_LOGD(TAG, "Temperature difference (%.2f°C) is above diff_mid (%.2f°C).", temp_sun - temp_watter, this->diff_mid_);
    if (this->pump_switch_ != nullptr) {
      this->pump_switch_->turn_on();
    }
  } else {
    ESP_LOGD(TAG, "Temperature conditions not met, turning off pump.");
    if (this->pump_switch_ != nullptr) {
      this->pump_switch_->turn_off();
    }
  }

  // Publicar el estado actualizado
  this->publish_state();
}

climate::ClimateTraits ClimateSolar::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(false);
  traits.set_visual_min_temperature(this->visual_min_temp_);
  traits.set_visual_max_temperature(this->visual_max_temp_);
  traits.set_supported_modes({climate::CLIMATE_MODE_OFF, climate::CLIMATE_MODE_HEAT});
  return traits;
}

void ClimateSolar::loop() {
  // Aquí puedes verificar y actualizar la lógica cada vez que cambien los valores de los sensores
  this->update_control_logic();
}

}  // namespace climate_solar
}  // namespace esphome
