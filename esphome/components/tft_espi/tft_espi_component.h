#pragma once

#include "esphome/core/component.h"
#include "TFT_eSPI.h"

namespace esphome {
namespace tft_e_spi {

class TFT_eSPIComponent : public esphome::Component {
 public:
  TFT_eSPIComponent(TFT_eSPI *tft) : tft_(tft) {}

  void setup() override {
    tft_->begin();
    tft_->setRotation(rotation_);
    tft_->fillScreen(TFT_BLACK);
  }

  void draw_pixel(int x, int y, uint16_t color) {
    tft_->drawPixel(x, y, color);
  }

  void fill_screen(uint16_t color) {
    tft_->fillScreen(color);
  }

  void print(int x, int y, const char* text, int size, uint16_t color) {
    tft_->setTextSize(size);
    tft_->setTextColor(color);
    tft_->setCursor(x, y);
    tft_->print(text);
  }

 private:
  TFT_eSPI *tft_;
  int rotation_ = 0;
};

}  // namespace tft_e_spi
}  // namespace esphome
