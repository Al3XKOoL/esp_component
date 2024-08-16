#include "esphome.h"
#include "climate_solar.h"

using namespace esphome;
using namespace esphome::climate_solar;

ClimateSolar *control_bomba;

void setup() {
  // Configuración inicial
  control_bomba = new ClimateSolar();
  App.register_component(control_bomba);
}
