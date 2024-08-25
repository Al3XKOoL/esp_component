#include "ili9341_parallel.h"

namespace esphome {
namespace ili9341_parallel {

void ILI9341ParallelDisplay::setup() {
  tft.init();
  tft.setRotation(rotation_);
}

void ILI9341ParallelDisplay::loop() {
  this->do_update_();
}

}  // namespace ili9341_parallel
}  // namespace esphome
