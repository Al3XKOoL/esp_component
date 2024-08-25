#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include <TFT_eSPI.h>

namespace esphome {
namespace tft_espi {

class TFTeSPIDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void display();
  void update() override;
  void fill(Color color) override;
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  void set_brightness(float brightness);

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

 protected:
  TFT_eSPI *tft_{nullptr};
};

}  // namespace tft_espi
}  // namespace esphome
