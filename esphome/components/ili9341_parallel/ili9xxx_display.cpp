#include "ili9xxx_display.h"
#include "ili9xxx_init.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9341";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGD(TAG, "Iniciando configuración...");
  this->init_lcd_();
  ESP_LOGD(TAG, "LCD inicializado");
  this->fill(Color::BLACK);
  ESP_LOGD(TAG, "Pantalla limpiada");
  ESP_LOGD(TAG, "Configuración completada");
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  ESP_LOGCONFIG(TAG, "  Model: %s", this->model_.c_str());
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
  ESP_LOGCONFIG(TAG, "  Data Pins: D0-D7");
  ESP_LOGCONFIG(TAG, "  Width: %d, Height: %d", this->width_, this->height_);
}

void ILI9341ParallelDisplay::update() {
  ESP_LOGD(TAG, "Iniciando actualización de pantalla");
  uint32_t start_time = millis();
  this->do_update_();
  uint32_t end_time = millis();
  ESP_LOGD(TAG, "Actualización completada en %u ms", end_time - start_time);
}

void ILI9341ParallelDisplay::fill(Color color) {
  ESP_LOGD(TAG, "Llenando pantalla con color: %d, %d, %d", color.r, color.g, color.b);
  this->set_addr_window_(0, 0, this->get_width_internal() - 1, this->get_height_internal() - 1);
  this->send_command_(ILI9XXX_RAMWR);
  for (uint32_t i = 0; i < this->get_width_internal() * this->get_height_internal(); i++) {
    this->write_color_(color);
  }
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  this->set_addr_window_(x, y, x, y);
  this->send_command_(ILI9XXX_RAMWR);
  this->write_color_(color);
}

void ILI9341ParallelDisplay::set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  this->send_command_(ILI9XXX_CASET);  // Column addr set
  this->send_data_(x1 >> 8);
  this->send_data_(x1 & 0xFF);  // XSTART
  this->send_data_(x2 >> 8);
  this->send_data_(x2 & 0xFF);  // XEND
  this->send_command_(ILI9XXX_PASET);  // Row addr set
  this->send_data_(y1 >> 8);
  this->send_data_(y1 & 0xFF);  // YSTART
  this->send_data_(y2 >> 8);
  this->send_data_(y2 & 0xFF);  // YEND
}

void ILI9341ParallelDisplay::write_color_(Color color) {
  uint16_t color565 = ((color.r & 0xF8) << 8) | ((color.g & 0xFC) << 3) | (color.b >> 3);
  this->send_data_(color565 >> 8);
  this->send_data_(color565 & 0xFF);
}

void ILI9341ParallelDisplay::init_lcd_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(true);
    delay(1);
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(120);
  }

  const uint8_t *addr = ILI9341_INIT_CMD;
  uint8_t cmd, num_args;
  while ((cmd = pgm_read_byte(addr++)) > 0) {
    num_args = pgm_read_byte(addr++);
    this->send_command_(cmd);
    for (uint8_t x = 0; x < num_args; x++) {
      this->send_data_(pgm_read_byte(addr++));
    }
    if (cmd == 0x01 || cmd == 0x11 || cmd == 0x29)
      delay(120);
  }
}

void ILI9341ParallelDisplay::send_command_(uint8_t cmd) {
  this->dc_pin_->digital_write(false);
  this->write_byte_(cmd);
}

void ILI9341ParallelDisplay::send_data_(uint8_t data) {
  this->dc_pin_->digital_write(true);
  this->write_byte_(data);
}

void ILI9341ParallelDisplay::write_byte_(uint8_t value) {
  for (uint8_t i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((value >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3,
                                           GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7) {
  this->data_pins_[0] = d0;
  this->data_pins_[1] = d1;
  this->data_pins_[2] = d2;
  this->data_pins_[3] = d3;
  this->data_pins_[4] = d4;
  this->data_pins_[5] = d5;
  this->data_pins_[6] = d6;
  this->data_pins_[7] = d7;
}

void ILI9341ParallelDisplay::set_data_pin(uint8_t index, GPIOPin *pin) {
  if (index < 8) {
    this->data_pins_[index] = pin;
  }
}

}  // namespace ili9xxx
}  // namespace esphome
