#include "tft_e_spi_component.h"

namespace esphome {
namespace tft_e_spi {

TFT_eSPIComponent::TFT_eSPIComponent() : tft_(TFT_eSPI()) {}

void TFT_eSPIComponent::setup() {
  // Inicializa el display
  tft_.begin();
  tft_.setRotation(rotation_);
  tft_.fillScreen(TFT_BLACK);
}

void TFT_eSPIComponent::update() {
  // Actualiza el display si es necesario
}

void TFT_eSPIComponent::fill_screen(uint16_t color) {
  tft_.fillScreen(color);
}

void TFT_eSPIComponent::print(int x, int y, const char* text, int size, uint16_t color) {
  tft_.setTextSize(size);
  tft_.setTextColor(color);
  tft_.setCursor(x, y);
  tft_.print(text);
}

void TFT_eSPIComponent::set_rotation(int rotation) {
  rotation_ = rotation;
  tft_.setRotation(rotation_);
}

}  // namespace tft_espi
}  // namespace esphome
