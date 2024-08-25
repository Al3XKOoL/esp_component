#include "tft_e_spi_display.h"

namespace esphome {
namespace tft_espi {

TFT_eSPI_Display::TFT_eSPI_Display() : tft_(TFT_eSPI()) {}

void TFT_eSPI_Display::setup() {
  tft_.begin();
  tft_.setRotation(rotation_);
  tft_.fillScreen(TFT_BLACK);
}

void TFT_eSPI_Display::update() {
  // Actualiza el display si es necesario
}

void TFT_eSPI_Display::fill_screen(uint16_t color) {
  tft_.fillScreen(color);
}

void TFT_eSPI_Display::print(int x, int y, const char* text, int size, uint16_t color) {
  tft_.setTextSize(size);
  tft_.setTextColor(color);
  tft_.setCursor(x, y);
  tft_.print(text);
}

void TFT_eSPI_Display::set_rotation(int rotation) {
  rotation_ = rotation;
  tft_.setRotation(rotation_);
}

}  // namespace tft_espi
}  // namespace esphome
