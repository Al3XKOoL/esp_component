#include "custom_climate.h"

namespace custom_climate {

void CustomClimate::setup() {
  // Inicialización si es necesaria
}

void CustomClimate::loop() {
  unsigned long current_time = millis();

  if (current_time - last_check_time_ >= interval_seconds_ * 1000) {
    last_check_time_ = current_time;

    ESP_LOGI("custom_climate", "Ejecutando loop()");

    // Obtener tiempo actual
    int64_t tiempo_actual = id(homeassistant_time).now().timestamp;
    if (tiempo_actual == 0) {
      tiempo_actual = id(sntp_time).now().timestamp;
    }

    if (espera_) {
      if (tiempo_actual >= tiempo_espera_fin_) {
        espera_ = false;
        ESP_LOGI("custom_climate", "Reanudando verificaciones después de espera de 5 minutos.");
      } else {
        ESP_LOGI("custom_climate", "En espera hasta %lld", tiempo_espera_fin_);
        return;
      }
    }

    bool estado_bomba_actual = pump_switch_->state;

    if (temp_sun_->state > (temp_water_->state + diff_high_) && temp_water_->state < temp_max_) {
      if (!estado_bomba_actual) {
        pump_switch_->turn_on();
        conteo_encendidos_++;
        tiempo_inicio_ = tiempo_actual;
        ESP_LOGI("custom_climate", "Bomba encendida debido a la temperatura adecuada");
      } else {
        ESP_LOGI("custom_climate", "Bomba ya está encendida");
      }
    } else {
      if (estado_bomba_actual) {
        pump_switch_->turn_off();
        int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio_;
        tiempo_encendida_ += tiempo_total_encendido;
        ESP_LOGI("custom_climate", "Bomba apagada debido a temperatura inadecuada");
        ESP_LOGW("custom_climate", "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);
        espera_ = true;
        tiempo_espera_fin_ = tiempo_actual + 300; // 5 minutos en segundos
        return;
      } else {
        ESP_LOGI("custom_climate", "Bomba ya está apagada");
        return;
      }
    }

    if (estado_bomba_actual && temp_output_->state < (temp_water_->state + 1)) {
      pump_switch_->turn_off();
      int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio_;
      tiempo_encendida_ += tiempo_total_encendido;
      ESP_LOGI("custom_climate", "Bomba apagada debido a temperatura de salida insuficiente");
      ESP_LOGW("custom_climate", "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);
      espera_ = true;
      tiempo_espera_fin_ = tiempo_actual + 300; // 5 minutos en segundos
      return;
    }

    if (estado_bomba_actual) {
      int64_t tiempo_transcurrido = tiempo_actual - tiempo_inicio_;
      ESP_LOGW("custom_climate", "Tiempo transcurrido de funcionamiento de la bomba: %02d:%02d:%02d",
                (int)(tiempo_transcurrido / 3600),
                (int)((tiempo_transcurrido % 3600) / 60),
                (int)(tiempo_transcurrido % 60));
    }

    ESP_LOGW("custom_climate", "Diferencia Sol-Agua: %.2f°C", temp_sun_->state - temp_water_->state);
    ESP_LOGW("custom_climate", "Diferencia Salida-Agua: %.2f°C", temp_output_->state - temp_water_->state);
    ESP_LOGW("custom_climate", "Estado de la bomba: %d", pump_switch_->state);
    ESP_LOGI("custom_climate", "Conteo de encendidos de la bomba: %d", conteo_encendidos_);

    this->publish_state();
  }
}

climate::ClimateTraits CustomClimate::traits() {
  auto traits = climate::ClimateTraits();
  traits.set_supports_current_temperature(true);
  traits.set_supports_two_point_target_temperature(true);
  traits.set_visual_min_temperature(visual_min_temp_);
  traits.set_visual_max_temperature(visual_max_temp_);
  traits.set_visual_temperature_step(0.1);
  return traits;
}

void CustomClimate::control(const climate::ClimateCall &call) {
  if (call.get_mode().has_value()) {
    this->mode = *call.get_mode();
  }
  if (call.get_target_temperature().has_value()) {
    this->target_temperature = *call.get_target_temperature();
  }
  this->publish_state();
}

}  // namespace custom_climate
