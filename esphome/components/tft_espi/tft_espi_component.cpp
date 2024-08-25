#include "tft_e_spi_component.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tft_espi {

TFT_eSPIComponent::TFT_eSPIComponent() : tft_(TFT_eSPI()) {}

void TFT_eSPIComponent::setup() {
  // Aquí configuras el display usando TFT_eSPI
  tft_.begin();
  tft_.setRotation(rotation_);
  tft_.fillScreen(TFT_BLACK);
}

void TFT_eSPIComponent::loop() {
  // Puedes agregar actualizaciones periódicas si es necesario
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
