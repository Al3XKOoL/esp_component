#include "ili9341_display.h"
#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ili9341_parallel {

static const char *const TAG = "ili9341_parallel";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGD(TAG, "Setting up ILI9341 Parallel Display...");
  this->init_pins_();
  this->init_lcd_();
  
  this->width_ = 240;
  this->height_ = 320;
  this->set_rotation(this->rotation_);
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  ESP_LOGCONFIG(TAG, "  Data pins: D0:%d D1:%d D2:%d D3:%d D4:%d D5:%d D6:%d D7:%d",
                this->data_pins_[0]->get_pin_number(), this->data_pins_[1]->get_pin_number(),
                this->data_pins_[2]->get_pin_number(), this->data_pins_[3]->get_pin_number(),
                this->data_pins_[4]->get_pin_number(), this->data_pins_[5]->get_pin_number(),
                this->data_pins_[6]->get_pin_number(), this->data_pins_[7]->get_pin_number());
  ESP_LOGCONFIG(TAG, "  Rotation: %d", this->rotation_);
  ESP_LOGCONFIG(TAG, "  Dimensions: %dpx x %dpx", this->width_, this->height_);
}

float ILI9341ParallelDisplay::get_setup_priority() const { return setup_priority::HARDWARE; }

void ILI9341ParallelDisplay::command(uint8_t value) {
  this->start_command_();
  this->write_byte(value);
  this->end_command_();
}

void ILI9341ParallelDisplay::data(uint8_t value) {
  this->start_data_();
  this->write_byte(value);
  this->end_data_();
}

void ILI9341ParallelDisplay::send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes) {
  this->command(command_byte);
  for (uint8_t i = 0; i < num_data_bytes; i++) {
    this->data(data_bytes[i]);
  }
}

void ILI9341ParallelDisplay::update() {
  this->do_update_();
  this->display_();
}

void ILI9341ParallelDisplay::display_() {
  uint16_t x1 = 0, y1 = 0, x2 = this->get_width_internal() - 1, y2 = this->get_height_internal() - 1;
  this->set_addr_window_(x1, y1, x2, y2);
  this->start_data_();
  for (uint32_t i = 0; i < this->get_buffer_length_(); i++) {
    this->write_byte(this->buffer_[i]);
  }
  this->end_data_();
}

void ILI9341ParallelDisplay::init_pins_() {
  this->dc_pin_->setup();
  this->dc_pin_->digital_write(false);
  this->wr_pin_->setup();
  this->wr_pin_->digital_write(true);
  this->rd_pin_->setup();
  this->rd_pin_->digital_write(true);
  this->cs_pin_->setup();
  this->cs_pin_->digital_write(true);
  
  for (uint8_t i = 0; i < 8; i++) {
    this->data_pins_[i]->setup();
    this->data_pins_[i]->digital_write(false);
  }
  
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
    this->reset_();
  }
}

void ILI9341ParallelDisplay::set_rotation(display::DisplayRotation rotation) {
  this->rotation_ = rotation;
  switch (rotation) {
    case display::DISPLAY_ROTATION_0_DEGREES:
      this->command(ILI9341_MADCTL);
      this->data(MADCTL_MX | MADCTL_BGR);
      this->width_ = 240;
      this->height_ = 320;
      break;
    case display::DISPLAY_ROTATION_90_DEGREES:
      this->command(ILI9341_MADCTL);
      this->data(MADCTL_MV | MADCTL_BGR);
      this->width_ = 320;
      this->height_ = 240;
      break;
    case display::DISPLAY_ROTATION_180_DEGREES:
      this->command(ILI9341_MADCTL);
      this->data(MADCTL_MY | MADCTL_BGR);
      this->width_ = 240;
      this->height_ = 320;
      break;
    case display::DISPLAY_ROTATION_270_DEGREES:
      this->command(ILI9341_MADCTL);
      this->data(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
      this->width_ = 320;
      this->height_ = 240;
      break;
  }
}

void ILI9341ParallelDisplay::start_command_() {
  this->dc_pin_->digital_write(false);
  this->cs_pin_->digital_write(false);
}

void ILI9341ParallelDisplay::end_command_() {
  this->cs_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::start_data_() {
  this->dc_pin_->digital_write(true);
  this->cs_pin_->digital_write(false);
}

void ILI9341ParallelDisplay::end_data_() {
  this->cs_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::write_byte(uint8_t data) {
  for (uint8_t i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((data >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(true);
    delay(1);
    this->reset_pin_->digital_write(false);
    delay(10);
    this->reset_pin_->digital_write(true);
    delay(120);
  }
}

void ILI9341ParallelDisplay::init_lcd_() {
  this->command(ILI9341_SWRESET);
  delay(120);
  
  this->command(ILI9341_DISPOFF);
  
  this->command(ILI9341_PWCTR1);
  this->data(0x23);
  
  this->command(ILI9341_PWCTR2);
  this->data(0x10);
  
  this->command(ILI9341_VMCTR1);
  this->data(0x3E);
  this->data(0x28);
  
  this->command(ILI9341_VMCTR2);
  this->data(0x86);
  
  this->set_rotation(this->rotation_);
  
  this->command(ILI9341_PIXFMT);
  this->data(0x55);
  
  this->command(ILI9341_FRMCTR1);
  this->data(0x00);
  this->data(0x18);
  
  this->command(ILI9341_DFUNCTR);
  this->data(0x08);
  this->data(0x82);
  this->data(0x27);
  
  this->command(ILI9341_SLPOUT);
  delay(120);
  
  this->command(ILI9341_DISPON);
}

void ILI9341ParallelDisplay::set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  this->command(ILI9341_CASET);
  this->data(x1 >> 8);
  this->data(x1);
  this->data(x2 >> 8);
  this->data(x2);
  
  this->command(ILI9341_PASET);
  this->data(y1 >> 8);
  this->data(y1);
  this->data(y2 >> 8);
  this->data(y2);
  
  this->command(ILI9341_RAMWR);
}

uint32_t ILI9341ParallelDisplay::get_buffer_length_() {
  return this->get_width_internal() * this->get_height_internal() * 2;
}

int ILI9341ParallelDisplay::get_width_internal() { return this->width_; }
int ILI9341ParallelDisplay::get_height_internal() { return this->height_; }

} // namespace ili9341_parallel
} // namespace esphome
