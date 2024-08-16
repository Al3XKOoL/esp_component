#pragma once

#include "esphome.h"

class CustomClimate : public Climate, public Component, public Interval {
 public:
  CustomClimate() : Interval(60000) {}  // Intervalo de 1 minuto

  void setup() override {
    tiempo_inicio = 0;
    espera = false;
    tiempo_espera_fin = 0;
    this->set_on_interval([this]() { this->loop(); });
  }

  void loop() override {
    auto tiempo_actual = id(homeassistant_time).now().timestamp;
    if (tiempo_actual == 0) {
      tiempo_actual = id(sntp_time).now().timestamp;
    }

    if (espera) {
      if (tiempo_actual >= tiempo_espera_fin) {
        espera = false;
        ESP_LOGI("main", "Reanudando verificaciones después de espera de 5 minutos.");
      } else {
        return;
      }
    }

    bool estado_bomba_actual = id(bomba_solar).state;

    if (id(temperatura_azotea).state > (id(temperatura_spa).state + 1.4) && id(temperatura_spa).state < id(temperatura_maxima)) {
      if (!estado_bomba_actual) {
        id(bomba_solar).turn_on();
        id(conteo_encendidos) += 1;
        tiempo_inicio = tiempo_actual;
        ESP_LOGI("main", "Bomba encendida debido a la temperatura adecuada");
      } else {
        ESP_LOGI("main", "Bomba ya está encendida");
      }
    } else {
      if (estado_bomba_actual) {
        id(bomba_solar).turn_off();
        int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio;
        id(tiempo_encendida) += tiempo_total_encendido;
        ESP_LOGI("main", "Bomba apagada debido a temperatura inadecuada");
        ESP_LOGW("main", "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);

        espera = true;
        tiempo_espera_fin = tiempo_actual + 300;
        return;
      } else {
        ESP_LOGI("main", "Bomba ya está apagada");
        return;
      }
    }

    if (estado_bomba_actual && id(temperatura_caliente).state < (id(temperatura_spa).state + 1)) {
      id(bomba_solar).turn_off();
      int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio;
      id(tiempo_encendida) += tiempo_total_encendido;
      ESP_LOGI("main", "Bomba apagada debido a temperatura de salida insuficiente");
      ESP_LOGW("main", "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);

      espera = true;
      tiempo_espera_fin = tiempo_actual + 300;
      return;
    }

    if (estado_bomba_actual) {
      int64_t tiempo_transcurrido = tiempo_actual - tiempo_inicio;
      ESP_LOGW("main", "Tiempo transcurrido de funcionamiento de la bomba: %02d:%02d:%02d",
                (int)(tiempo_transcurrido / 3600),
                (int)((tiempo_transcurrido % 3600) / 60),
                (int)(tiempo_transcurrido % 60));
    }

    ESP_LOGW("main", "Diferencia Azotea-SPA: %.2f°C", id(temperatura_azotea).state - id(temperatura_spa).state);
    ESP_LOGW("main", "Diferencia Caliente-SPA: %.2f°C", id(temperatura_caliente).state - id(temperatura_spa).state);
    ESP_LOGW("main", "Estado de la bomba: %d", id(bomba_solar).state);

    ESP_LOGI("main", "Conteo de encendidos de la bomba: %d", id(conteo_encendidos));
  }

  void control(const ClimateCall &call) override {
    // Implementar el control aquí si es necesario
  }

 private:
  int64_t tiempo_inicio;
  bool espera;
  int64_t tiempo_espera_fin;
};
