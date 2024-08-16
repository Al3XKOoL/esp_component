// external_components/bomba_automatica/bomba_automatica.cpp

#include "bomba_automatica.h"
#include "esphome/core/log.h"

namespace esphome {
namespace bomba_automatica {

static const char *TAG = "bomba_automatica";

void BombaAutomatica::setup() {
  // Configura la verificación periódica
  this->check_bomba_status();
}

void BombaAutomatica::loop() {
  // Llama al método de verificación periódicamente
  this->check_bomba_status();
}

void BombaAutomatica::check_bomba_status() {
  auto tiempo_actual = millis() / 1000; // Obtener tiempo actual en segundos
  
  if (espera) {
    if (tiempo_actual >= tiempo_espera_fin) {
      espera = false;
      ESP_LOGI(TAG, "Reanudando verificaciones después de espera.");
    } else {
      return; // Salir si estamos en espera
    }
  }

  bool estado_bomba_actual = id(bomba_solar).state;

  // Verificar condiciones de la bomba y tomar acciones
  if (id(temperatura_azotea).state > (id(temperatura_spa).state + 1.4) && id(temperatura_spa).state < id(temperatura_maxima)) {
    if (!estado_bomba_actual) {
      id(bomba_solar).turn_on();
      id(conteo_encendidos) += 1;
      tiempo_inicio = tiempo_actual;
      ESP_LOGI(TAG, "Bomba encendida debido a la temperatura adecuada.");
    } else {
      ESP_LOGI(TAG, "Bomba ya está encendida.");
    }
  } else {
    if (estado_bomba_actual) {
      id(bomba_solar).turn_off();
      int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio;
      id(tiempo_encendida) += tiempo_total_encendido; // Actualizar tiempo total en segundos
      ESP_LOGI(TAG, "Bomba apagada debido a temperatura inadecuada.");
      ESP_LOGW(TAG, "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);

      espera = true;
      tiempo_espera_fin = tiempo_actual + espera_time_; // Tiempo de espera configurado
      return; // Salir del método mientras estamos en espera
    } else {
      ESP_LOGI(TAG, "Bomba ya está apagada.");
      return;
    }
  }

  // Verificar temperatura de salida
  if (estado_bomba_actual && id(temperatura_caliente).state < (id(temperatura_spa).state + 1)) {
    id(bomba_solar).turn_off();
    int64_t tiempo_total_encendido = tiempo_actual - tiempo_inicio;
    id(tiempo_encendida) += tiempo_total_encendido; // Actualizar tiempo total en segundos
    ESP_LOGI(TAG, "Bomba apagada debido a temperatura de salida insuficiente.");
    ESP_LOGW(TAG, "Tiempo total de funcionamiento de la bomba: %lld segundos", tiempo_total_encendido);

    espera = true;
    tiempo_espera_fin = tiempo_actual + espera_time_; // Tiempo de espera configurado
    return;
  }

  // Actualizar y mostrar tiempo de funcionamiento
  if (estado_bomba_actual) {
    int64_t tiempo_transcurrido = tiempo_actual - tiempo_inicio;
    ESP_LOGW(TAG, "Tiempo transcurrido de funcionamiento de la bomba: %02d:%02d:%02d", 
              (int)(tiempo_transcurrido / 3600), 
              (int)((tiempo_transcurrido % 3600) / 60), 
              (int)(tiempo_transcurrido % 60));
  }

  // Logging adicional para depuración
  ESP_LOGW(TAG, "Diferencia Azotea-SPA: %.2f°C", id(temperatura_azotea).state - id(temperatura_spa).state);
  ESP_LOGW(TAG, "Diferencia Caliente-SPA: %.2f°C", id(temperatura_caliente).state - id(temperatura_spa).state);
  ESP_LOGW(TAG, "Estado de la bomba: %d", id(bomba_solar).state);

  // Mostrar el conteo de encendidos
  ESP_LOGI(TAG, "Conteo de encendidos de la bomba: %d", id(conteo_encendidos));
}

}  // namespace bomba_automatica
}  // namespace esphome
