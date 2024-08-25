#include "tft_espi.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tft_espi {

static const char *const TAG = "tft_espi";

TFTeSPIDisplay::TFTeSPIDisplay() : buffer_(nullptr), tft_(nullptr) {}

TFTeSPIDisplay::~TFTeSPIDisplay() {
  if (buffer_) {
    delete[] buffer_;
  }
  if (tft_) {
    delete tft_;
  }
}

void TFTeSPIDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TFT display...");
  tft_ = new TFT_eSPI();  // Configura el TFT
  tft_->init();
  // Configura el TFT como sea necesario
}

void TFTeSPIDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "TFTeSPIDisplay configuration:");
  // Agrega más detalles de configuración si es necesario
}

float TFTeSPIDisplay::get_setup_priority() const {
  return esphome::setup_priority::HARDWARE;
}

void TFTeSPIDisplay::fill(Color color) {
  tft_->fillScreen(display::ColorUtil::color_to_565(color));
}

void TFTeSPIDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  tft_->drawPixel(x, y, display::ColorUtil::color_to_565(color));
}

int TFTeSPIDisplay::get_width_internal() {
  return tft_->width();
}

int TFTeSPIDisplay::get_height_internal() {
  return tft_->height();
}

void TFTeSPIDisplay::update() {
  // Implementa el método de actualización si es necesario
}

esphome::display::DisplayType TFTeSPIDisplay::get_display_type() {
  // Si DisplayType no está definido, podrías dejar esto sin implementar o usar un valor predeterminado
  return esphome::display::DisplayType::UNKNOWN;  // O el tipo adecuado según tu configuración
}

}  // namespace tft_espi
}  // namespace esphome
