// external_components/bomba_automatica/bomba_automatica.h

#pragma once

#include "esphome/core/component.h"

namespace esphome {
namespace bomba_automatica {

class BombaAutomatica : public Component {
 public:
  void setup() override;
  void loop() override;

  void set_bomba_id(const std::string &id) { bomba_id_ = id; }
  void set_espera_time(int wait_time) { espera_time_ = wait_time; }

 private:
  std::string bomba_id_;
  int espera_time_ = 300;
  int64_t tiempo_inicio = 0;
  bool espera = false;
  int64_t tiempo_espera_fin = 0;

  void check_bomba_status();
};

}  // namespace bomba_automatica
}  // namespace esphome
