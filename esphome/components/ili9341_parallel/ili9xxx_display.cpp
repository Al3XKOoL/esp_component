#include "ili9xxx_display.h"
#include "ili9xxx_defines.h"
#include "ili9xxx_init.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/display/display_color_utils.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9xxx";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGD(TAG, "Setting up ILI9341 Parallel Display");
  
  this->init_pins_();
  this->init_lcd_();
  
  this->buffer_ = new uint8_t[this->get_width_internal() * this->get_height_internal() * 3];
  if (this->buffer_ == nullptr) {
    ESP_LOGE(TAG, "Failed to allocate display buffer");
    this->mark_failed();
    return;
  }
  
  memset(this->buffer_, 0, this->get_width_internal() * this->get_height_internal() * 3);
}

void ILI9341ParallelDisplay::init_lcd_() {
  ESP_LOGD(TAG, "Initializing ILI9341 Parallel Display");

  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(true);
    delay(5);
    this->reset_pin_->digital_write(false);
    delay(20);
    this->reset_pin_->digital_write(true);
    delay(150);
  }

  const uint8_t *addr = ILI9341_INIT_CMD;
  while (true) {
    uint8_t cmd = pgm_read_byte(addr++);
    uint8_t num_args = pgm_read_byte(addr++);
    if (cmd == 0x00) break;

    this->send_command_(cmd);
    for (uint8_t i = 0; i < num_args; i++) {
      this->send_data_(pgm_read_byte(addr++));
    }

    if (num_args & 0x80) {
      delay(150);
    }
  }

  ESP_LOGD(TAG, "ILI9341 Parallel Display initialized");
}

void ILI9341ParallelDisplay::set_rotation(uint8_t rotation) {
  this->rotation_ = rotation % 4;
  uint8_t madctl = 0;

  switch (this->rotation_) {
    case 0:
      madctl = MADCTL_MX | MADCTL_BGR;
      this->width_ = this->width_internal_;
      this->height_ = this->height_internal_;
      break;
    case 1:
      madctl = MADCTL_MV | MADCTL_BGR;
      this->width_ = this->height_internal_;
      this->height_ = this->width_internal_;
      break;
    case 2:
      madctl = MADCTL_MY | MADCTL_BGR;
      this->width_ = this->width_internal_;
      this->height_ = this->height_internal_;
      break;
    case 3:
      madctl = MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR;
      this->width_ = this->height_internal_;
      this->height_ = this->width_internal_;
      break;
  }

  ESP_LOGD(TAG, "Setting rotation: %d", this->rotation_);
  ESP_LOGD(TAG, "MADCTL value: 0x%02X", madctl);

  this->send_command_(ILI9XXX_MADCTL);
  this->send_data_(madctl);
}

void ILI9341ParallelDisplay::write_byte_(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((value >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  this->wr_pin_->digital_write(true);
}

void ILI9341ParallelDisplay::send_command_(uint8_t cmd) {
  if (this->dc_pin_ == nullptr || this->wr_pin_ == nullptr) {
    ESP_LOGE(TAG, "DC or WR pin not set. Cannot send command.");
    return;
  }
  this->dc_pin_->digital_write(false);
  this->write_byte_(cmd);
}

void ILI9341ParallelDisplay::send_data_(uint8_t data) {
  if (this->dc_pin_ == nullptr || this->wr_pin_ == nullptr) {
    ESP_LOGE(TAG, "DC or WR pin not set. Cannot send data.");
    return;
  }
  this->dc_pin_->digital_write(true);
  this->write_byte_(data);
}

void ILI9341ParallelDisplay::update() {
  if (!this->writer_.has_value())
    return;
  
  this->do_update_();
  this->write_display_();
}

void ILI9341ParallelDisplay::write_display_() {
  uint16_t buffer_width = this->get_width_internal();
  uint16_t buffer_height = this->get_height_internal();

  this->set_addr_window_(0, 0, buffer_width - 1, buffer_height - 1);
  this->send_command_(ILI9XXX_RAMWR);

  uint16_t line_buffer[buffer_width];

  for (int y = 0; y < buffer_height; y++) {
    for (int x = 0; x < buffer_width; x++) {
      Color color = this->get_pixel_color(x, y);
      line_buffer[x] = display::ColorUtil::color_to_565(color);
    }

    for (int x = 0; x < buffer_width; x++) {
      this->send_data_(line_buffer[x] >> 8);
      this->send_data_(line_buffer[x] & 0xFF);
    }
  }
}

void ILI9341ParallelDisplay::fill(Color color) {
  ESP_LOGD(TAG, "Filling screen with color: %d, %d, %d", color.r, color.g, color.b);
  
  uint16_t color565 = display::ColorUtil::color_to_565(color);
  uint16_t buffer[32];
  for (int i = 0; i < 32; i++) {
    buffer[i] = color565;
  }

  this->set_addr_window_(0, 0, this->get_width_internal() - 1, this->get_height_internal() - 1);
  
  for (int i = 0; i < this->get_width_internal() * this->get_height_internal(); i += 32) {
    this->send_command_(ILI9XXX_RAMWR);
    for (int j = 0; j < 32; j++) {
      this->send_data_(buffer[j] >> 8);
      this->send_data_(buffer[j] & 0xFF);
    }
  }
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  ESP_LOGCONFIG(TAG, "  Width: %d, Height: %d", this->width_, this->height_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
  for (int i = 0; i < 8; i++) {
    ESP_LOGCONFIG(TAG, "  Data Pin %d: %s", i, this->data_pins_[i] ? this->data_pins_[i]->dump_summary().c_str() : "None");
  }
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

Color ILI9341ParallelDisplay::get_pixel_color(int x, int y) {
  if (x < 0 || x >= this->get_width_internal() || y < 0 || y >= this->get_height_internal()) {
    return Color::BLACK;
  }
  uint32_t index = (y * this->get_width_internal() + x) * 3;
  return Color(this->buffer_[index], this->buffer_[index + 1], this->buffer_[index + 2]);
}

ILI9341ParallelDisplay::~ILI9341ParallelDisplay() {
  if (this->buffer_ != nullptr) {
    delete[] this->buffer_;
  }
}

ILI9341ParallelDisplay::ILI9341ParallelDisplay() : display::DisplayBuffer() {
}

void ILI9341ParallelDisplay::init_pins_() {
  for (int i = 0; i < 8; i++) {
    if (this->data_pins_[i] != nullptr) {
      this->data_pins_[i]->setup();
    }
  }
  
  if (this->dc_pin_ != nullptr) {
    this->dc_pin_->setup();
  }
  
  if (this->wr_pin_ != nullptr) {
    this->wr_pin_->setup();
  }
  
  if (this->rd_pin_ != nullptr) {
    this->rd_pin_->setup();
  }
  
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->setup();
  }
  
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->setup();
  }
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || y >= this->get_height_internal() || x < 0 || y < 0)
    return;

  this->set_addr_window_(x, y, x, y);
  this->send_command_(ILI9XXX_RAMWR);
  this->write_color_(color);
}

void ILI9341ParallelDisplay::set_width(uint16_t width) {
  this->width_ = width;
}

void ILI9341ParallelDisplay::set_height(uint16_t height) {
  this->height_ = height;
}

void ILI9341ParallelDisplay::set_data_pin(uint8_t index, GPIOPin *pin) {
  if (index < 8) {
    this->data_pins_[index] = pin;
  }
}

void ILI9341ParallelDisplay::write_color_(Color color) {
  this->write_byte_(color.red);
  this->write_byte_(color.green);
  this->write_byte_(color.blue);
}

}  // namespace ili9xxx
}  // namespace esphome
