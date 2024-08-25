#include "tft_espi.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tft_espi {

static const char *TAG = "tft_espi";

TFTeSPIDisplay::TFTeSPIDisplay() : buffer_(nullptr) {}
TFTeSPIDisplay::~TFTeSPIDisplay() {
  delete this->tft_;
  delete[] this->buffer_;
}

void TFTeSPIDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TFT eSPI Display...");
  this->tft_ = new TFT_eSPI();
  this->tft_->init();
  this->tft_->fillScreen(TFT_BLACK);
}

void TFTeSPIDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "TFT eSPI Display:");
  LOG_UPDATE_INTERVAL(this);
}

float TFTeSPIDisplay::get_setup_priority() const {
  return setup_priority::PROCESSOR;
}

void TFTeSPIDisplay::display() {
  if (this->auto_clear_) {
    this->tft_->fillScreen(TFT_BLACK);
  }
  this->tft_->startWrite();
  for (int y = 0; y < this->get_height_internal(); y++) {
    for (int x = 0; x < this->get_width_internal(); x++) {
      this->tft_->drawPixel(x, y, display::ColorUtil::color_to_565(this->get_pixel_color(x, y)));
    }
  }
  this->tft_->endWrite();
}

void TFTeSPIDisplay::set_brightness(float brightness) {
  ESP_LOGW(TAG, "Brightness control not implemented for this display");
}

void TFTeSPIDisplay::fill(Color color) {
  this->tft_->fillScreen(display::ColorUtil::color_to_565(color));
}

void TFTeSPIDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  this->tft_->drawPixel(x, y, display::ColorUtil::color_to_565(color));
}

int TFTeSPIDisplay::get_width_internal() { return this->tft_->width(); }
int TFTeSPIDisplay::get_height_internal() { return this->tft_->height(); }

Color TFTeSPIDisplay::get_pixel_color(int x, int y) {
  // Implementa la función aquí
  return Color(0, 0, 0); // Un valor por defecto, implementa la lógica real
}

esphome::display::DisplayType TFTeSPIDisplay::get_display_type() {
  return esphome::display::DisplayType::TFT;  // Ajusta esto según los valores válidos en display.h
}

void TFTeSPIDisplay::update() {
  this->do_update_();
  this->display();
}

}  // namespace tft_espi
}  // namespace esphome
