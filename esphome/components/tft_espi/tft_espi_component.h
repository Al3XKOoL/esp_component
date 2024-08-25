#pragma once

#include "esphome/core/component.h"
#include "TFT_eSPI.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace tft_espi {

class TFT_eSPI_Display : public esphome::Component, public esphome::display::DisplayBuffer {
 public:
  TFT_eSPI_Display();

  void setup() override;
  void update() override;

  void fill_screen(uint16_t color);
  void print(int x, int y, const char* text, int size, uint16_t color);

  void set_rotation(int rotation);

 private:
  TFT_eSPI tft_;
  int rotation_ = 0;
};

}  // namespace tft_espi
}  // namespace esphome
