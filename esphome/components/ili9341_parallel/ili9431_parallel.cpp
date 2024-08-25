#include "ili9341_parallel.h"

ILI9341ParallelDisplay display;

void setup() {
  display.setup();
}

void loop() {
  display.update();
}
