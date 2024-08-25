#include "tft_espi.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace tft_espi {

static const char *const TAG = "tft_espi";

void TFTeSPIDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up TFT eSPI Display...");
  this->tft_ = new TFT_eSPI();
  this->tft_->init();
  this->tft_->setRotation(0);  // Ajusta según sea necesario

  // Informa a ESPHome sobre las dimensiones de la pantalla
  this->set_geometry(this->tft_->width(), this->tft_->height(), 1);
}

void TFTeSPIDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "TFT eSPI Display:");
  LOG_DISPLAY("  ", "TFT eSPI", this);
  ESP_LOGCONFIG(TAG, "  Width: %d, Height: %d", this->get_width_internal(), this->get_height_internal());
}

float TFTeSPIDisplay::get_setup_priority() const { return setup_priority::PROCESSOR; }

void TFTeSPIDisplay::update() {
  this->do_update_();
}

void TFTeSPIDisplay::fill(Color color) {
  this->tft_->fillScreen(display::ColorUtil::color_to_565(color));
}

void HOT TFTeSPIDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  this->tft_->drawPixel(x, y, display::ColorUtil::color_to_565(color));
}

int TFTeSPIDisplay::get_width_internal() { return this->tft_->width(); }
int TFTeSPIDisplay::get_height_internal() { return this->tft_->height(); }

void TFTeSPIDisplay::set_brightness(float brightness) {
  // Implementa el control de brillo si tu pantalla lo soporta
  // Por ejemplo, podrías usar PWM si tu pantalla tiene un pin de control de brillo
  ESP_LOGW(TAG, "Brightness control not implemented for this display");
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

size_t TFTeSPIDisplay::get_buffer_length_() {
  return size_t(this->get_width_internal()) * size_t(this->get_height_internal()) * 2;
}

void TFTeSPIDisplay::write_display_data_() {
  this->tft_->startWrite();
  this->tft_->pushColors(this->buffer_, this->get_buffer_length_(), true);
  this->tft_->endWrite();
}

}  // namespace tft_espi
}  // namespace esphome
