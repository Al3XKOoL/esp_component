#include "tft_espi.h"
#include "esphome/core/log.h"

namespace esphome {
namespace tft_espi {

static const char *TAG = "tft_espi";

void TFTeSPIDisplay::setup() {
  this->tft_ = new TFT_eSPI();
  this->tft_->init();
  this->tft_->setRotation(0);  // Ajusta según sea necesario
  
  // Configura el buffer de la pantalla
  this->initialize_();
}

void TFTeSPIDisplay::display() {
  this->tft_->startWrite();
  for (int y = 0; y < this->get_height_internal(); y++) {
    for (int x = 0; x < this->get_width_internal(); x++) {
      Color color = this->get_pixel_color(x, y);
      this->tft_->drawPixel(x, y, color.to_565());
    }
  }
  this->tft_->endWrite();
}

void TFTeSPIDisplay::update() {
  this->do_update_();
  this->display();
}

void TFTeSPIDisplay::fill(Color color) {
  this->tft_->fillScreen(color.to_565());
}

void TFTeSPIDisplay::draw_pixel_at(int x, int y, Color color) {
  this->tft_->drawPixel(x, y, color.to_565());
}

void TFTeSPIDisplay::set_brightness(float brightness) {
  // Implementa el control de brillo si tu pantalla lo soporta
  // Por ejemplo, podrías usar PWM si tu pantalla tiene un pin de control de brillo
}

}  // namespace tft_espi
}  // namespace esphome
