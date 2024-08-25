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
  this->tft_->setRotation(0);  // Ajusta según sea necesario
}

void TFTeSPIDisplay::display() {
  this->tft_->startWrite();
  for (int y = 0; y < this->get_height_internal(); y++) {
    for (int x = 0; x < this->get_width_internal(); x++) {
      Color color = this->get_pixel_color(x, y);
      this->draw_absolute_pixel_internal(x, y, color);
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
  // Implementa el control de brillo si tu pantalla lo soporta
  ESP_LOGW(TAG, "Brightness control not implemented for this display");
}

}  // namespace tft_espi
}  // namespace esphome
