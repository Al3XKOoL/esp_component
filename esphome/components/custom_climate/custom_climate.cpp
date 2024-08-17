#include "custom_climate.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace custom_climate {

static const char *const TAG = "custom_climate";

void CustomClimate::setup() {
  // Initialization if needed
}

void CustomClimate::loop() {
  unsigned long current_time = millis();

  if (current_time - last_check_time_ >= interval_seconds_ * 1000) {
    last_check_time_ = current_time;

    ESPHOME_LOGD("Ejecutando loop()");

    // Get current time
    int64_t tiempo_actual = 0;
    if (homeassistant_time_ != nullptr) {
      tiempo_actual = homeassistant_time_->now().timestamp;
    }
    if (tiempo_actual == 0 && sntp_time_ != nullptr) {
      tiempo_actual = sntp_time_->now().timestamp;
    }

    if (espera_) {
      if (tiempo_actual >= tiempo_espera_fin_) {
        espera_ = false;
        ESPHOME_LOGD("Reanudando verificaciones después de espera de 5 minutos.");
      } else {
        ESPHOME_LOGD("En espera hasta %lld", tiempo_espera_fin_);
        return;
      }
    }

    bool estado_bomba_actual = pump_switch_->state;

    if (temp_sun_->state > (temp_water_->state + diff_high_) && temp_water_->state < temp_max_) {
      if (!estado_bomba_actual) {
        pump_switch_->turn_on();
        conteo_encendidos_++;
        tiempo_inicio_ = tiempo_actual;
        ESPHOME_LOGD("Bomba encendida debido a la temperatura adecuada");
      } else {
        ESPHOME_LOGD("Bomba ya está encendida");
      }
    } else {
      if (estado_bomba_actual) {
        pump_switch_->turn_off();
        int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio_;
        tiempo_encendida_ += tiempo_total_encendido;
        ESPHOME_LOGD("Bomba apagada debido a temperatura inadecuada");
        ESPHOME_LOGW("Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);
        espera_ = true;
        tiempo_espera_fin_ = tiempo_actual + 300; // 5 minutes in seconds
        return;
      } else {
        ESPHOME_LOGD("Bomba ya está apagada");
        return;
      }
    }

    if (estado_bomba_actual && temp_output_->state < (temp_water_->state + 1)) {
      pump_switch_->turn_off();
      int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio_;
      tiempo_encendida_ += tiempo_total_encendido;
      ESPHOME_LOGD("Bomba apagada debido a temperatura de salida insuficiente");
      ESPHOME_LOGW("Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);
      espera_ = true;
      tiempo_espera_fin_ = tiempo_actual + 300; // 5 minutes in seconds
      return;
    }

    if (estado_bomba_actual) {
      int64_t tiempo_transcurrido = tiempo_actual - tiempo_inicio_;
      ESPHOME_LOGW("Tiempo transcurrido de funcionamiento de la bomba: %02d:%02d:%02d",
               (int)(tiempo_transcurrido / 3600),
               (int)((tiempo_transcurrido % 3600) / 60),
               (int)(tiempo_transcurrido % 60));
    }

    ESPHOME_LOGW("Diferencia Sol-Agua: %.2f°C", temp_sun_->state - temp_water_->state);
    ESPHOME_LOGW("Diferencia Salida-Agua: %.2f°C", temp_output_->state - temp_water_->state);
    ESPHOME_LOGW("Estado de la bomba: %d", pump_switch_->state);
    ESPHOME_LOGD("Conteo de encendidos de la bomba: %d", conteo_encendidos_);

    this->publish_state();
  }
}

esphome::climate::ClimateTraits CustomClimate::traits() {
  auto traits = esphome::climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(true);
  traits.set_visual_min_temperature(visual_min_temp_);
  traits.set_visual_max_temperature(visual_max_temp_);
  traits.set_visual_temperature_step(0.1);
  return traits;
}

void CustomClimate::control(const esphome::climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    this->mode = *call.get_mode();
  }
  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
  }
  this->publish_state();
}

}  // namespace custom_climate
