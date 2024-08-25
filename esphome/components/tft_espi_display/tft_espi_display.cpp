#include "tft_espi_display.h"

void TftEspiComponent::setup() {
  this->tft.init();
  this->tft.setSwapBytes(true);
  // Otras inicializaciones necesarias
}

void TftEspiComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "TFT eSPI:");
  LOG_UPDATE_INTERVAL(this);
  // Añade más información de configuración si es necesario
}

float TftEspiComponent::get_setup_priority() const { return setup_priority::PROCESSOR; }

void TftEspiComponent::display() {
  // Implementa la lógica de actualización de la pantalla
}

void TftEspiComponent::draw_absolute_pixel_internal(int x, int y, Color color) {
  this->tft.drawPixel(x, y, display::ColorUtil::color_to_565(color));
}

int TftEspiComponent::get_width_internal() { return this->tft.width(); }
int TftEspiComponent::get_height_internal() { return this->tft.height(); }

void TftEspiComponent::set_rotation(uint8_t rotation) {
  this->tft.setRotation(rotation);
}

void TftEspiComponent::set_invert(bool invert) {
  this->tft.invertDisplay(invert);
}
