#include "esphome.h"
#include "climate_solar.h"

ClimateSolar *control_bomba;

void setup() {
  control_bomba = new ClimateSolar();
  App.register_component(control_bomba);
  // Configurar sensores, switches, etc. si es necesario
}
