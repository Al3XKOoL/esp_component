#include "tft_espi.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace tft_espi {

static const char *TAG = "tft_espi";

void TFTeSPIDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TFT eSPI Display...");
  this->tft_ = new TFT_eSPI();
  this->tft_->init();
  this->tft_->fillScreen(TFT_BLACK);
}

Color TFTeSPIDisplay::get_pixel_color(int x, int y) {
  if (this->buffer_ == nullptr) {
    ESP_LOGE(TAG, "Buffer not initialized");
    return Color(0, 0, 0);
  }
  if (x < 0 || x >= this->get_width_internal() || y < 0 || y >= this->get_height_internal()) {
    ESP_LOGE(TAG, "Coordinates out of bounds");
    return Color(0, 0, 0);
  }
  int index = (y * this->get_width_internal() + x) * 3;
  return Color(this->buffer_[index], this->buffer_[index + 1], this->buffer_[index + 2]);
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

void TFTeSPIDisplay::update() {
  this->do_update_();
  this->display();
}

void TFTeSPIDisplay::fill(Color color) {
  this->tft_->fillScreen(display::ColorUtil::color_to_565(color));
}

void TFTeSPIDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  this->tft_->drawPixel(x, y, display::ColorUtil::color_to_565(color));
}

int TFTeSPIDisplay::get_width_internal() { return this->tft_->width(); }
int TFTeSPIDisplay::get_height_internal() { return this->tft_->height(); }

void TFTeSPIDisplay::set_brightness(float brightness) {
  ESP_LOGW(TAG, "Brightness control not implemented for this display");
}

}  // namespace tft_espi
}  // namespace esphome
