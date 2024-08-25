#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "TFT_eSPI.h"

namespace esphome {
namespace tft_espi {

class TFT_eSPIComponent : public esphome::Component, public esphome::display::DisplayBuffer {
public:
  TFT_eSPIComponent(TFT_eSPI* tft) : tft_(tft) {}

  void setup() override {
    tft_->begin();
    tft_->setRotation(rotation_);
    tft_->fillScreen(TFT_BLACK);
  }

  void draw_pixel(int x, int y, color_t color) override {
    tft_->drawPixel(x, y, color);
  }

  // Otros métodos necesarios según la aplicación

private:
  TFT_eSPI* tft_;
  int rotation_ = 0;
};

}  // namespace tft_espi
}  // namespace esphome
