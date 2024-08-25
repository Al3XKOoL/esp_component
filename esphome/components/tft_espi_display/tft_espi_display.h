#pragma once

#include "esphome.h"
#include "TFT_eSPI.h"

class TftEspiComponent : public Component, public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void display();
  
  void set_rotation(uint8_t rotation);
  void set_invert(bool invert);
  
 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  
 private:
  TFT_eSPI tft;
};
