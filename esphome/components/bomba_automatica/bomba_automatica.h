// external_components/bomba_automatica/bomba_automatica.h

#pragma once

#include "esphome/core/component.h"
#include "esphome/core/automation.h"

namespace esphome {
namespace bomba_automatica {

class BombaAutomatica : public Component {
 public:
  void setup() override;
  void loop() override;

  // Configuración de parámetros
  void set_bomba_id(const std::string &id) { bomba_id_ = id; }
  void set_espera(int wait_time) { espera_time_ = wait_time; }

  static void register_bomba_automatica(esp32::Component *component) {
    // Registramos el componente en el esquema de ESPHome
    auto *c = static_cast<BombaAutomatica*>(component);
    ESP_LOGD(TAG, "Registrando Bomba Automatica");
    c->setup();
  }

 protected:
  std::string bomba_id_;
  int espera_time_ = 300; // Tiempo de espera por defecto (en segundos)
  int64_t tiempo_inicio = 0;
  bool espera = false;
  int64_t tiempo_espera_fin = 0;
  
  // Métodos privados para la lógica
  void check_bomba_status();
};

}  // namespace bomba_automatica
}  // namespace esphome
