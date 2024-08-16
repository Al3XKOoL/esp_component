#include "esphome.h"
#include "climate_solar.h"

ClimateSolar *control_bomba;

void setup() {
  // Configuración inicial
  control_bomba = new ClimateSolar();
  App.register
