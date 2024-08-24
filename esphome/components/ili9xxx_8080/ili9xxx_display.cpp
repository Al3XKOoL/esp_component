#include "ili9xxx_display.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "ili9xxx_defines.h"
#include "ili9xxx_init.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9xxx";

void ILI9341ParallelDisplay::setup() {
  this->init_internal_(this->get_buffer_length_());
  this->init_display();
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  for (int i = 0; i < 8; i++) {
    LOG_PIN("  DATA Pin %d: ", i, this->data_pins_[i]);
  }
  LOG_DISPLAY_BUFFER("  ", "Display Buffer");
}

void ILI9341ParallelDisplay::init_display() {
  this->reset_pin_->digital_write(true);
  delay(1);
  this->reset_pin_->digital_write(false);
  delay(10);
  this->reset_pin_->digital_write(true);
  delay(120);

  for (uint8_t cmd : INITCMD_ILI9341) {
    this->write_command(cmd);
  }
}

void ILI9341ParallelDisplay::write_command(uint8_t command) {
  this->dc_pin_->digital_write(false);
  this->wr_pin_->digital_write(false);
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((command >> i) & 0x01);
  }
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::write_data(uint8_t data) {
  this->dc_pin_->digital_write(true);
  this->wr_pin_->digital_write(false);
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((data >> i) & 0x01);
  }
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::write_array(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    this->write_data(data[i]);
  }
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
    return;

  this->write_command(ILI9XXX_CASET);
  this->write_data(x >> 8);
  this->write_data(x);
  this->write_data(x >> 8);
  this->write_data(x);

  this->write_command(ILI9XXX_PASET);
  this->write_data(y >> 8);
  this->write_data(y);
  this->write_data(y >> 8);
  this->write_data(y);

  this->write_command(ILI9XXX_RAMWR);
  this->write_data(color.red);
  this->write_data(color.green);
  this->write_data(color.blue);
}

void ILI9341ParallelDisplay::display() {
  this->write_command(ILI9XXX_CASET);
  this->write_data(0);
  this->write_data(0);
  this->write_data((this->get_width_internal() - 1) >> 8);
  this->write_data((this->get_width_internal() - 1) & 0xFF);
  this->write_command(ILI9XXX_PASET);
  this->write_data(0);
  this->write_data(0);
  this->write_data((this->get_height_internal() - 1) >> 8);
  this->write_data((this->get_height_internal() - 1) & 0xFF);

  this->write_command(ILI9XXX_RAMWR);
  this->write_array(this->buffer_, this->get_buffer_length_());
}

}  // namespace ili9xxx
}  // namespace esphome
