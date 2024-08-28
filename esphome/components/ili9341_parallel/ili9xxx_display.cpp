#include "ili9xxx_display.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace ili9xxx {

static const char *const TAG = "ili9341_parallel";

void ILI9341ParallelDisplay::setup() {
  ESP_LOGCONFIG(TAG, "Setting up ILI9341 Parallel Display...");
  this->init_lcd_();
  this->set_rotation(this->rotation_);
}

void ILI9341ParallelDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "ILI9341 Parallel Display:");
  LOG_PIN("  Reset Pin: ", this->reset_pin_);
  LOG_PIN("  CS Pin: ", this->cs_pin_);
  LOG_PIN("  DC Pin: ", this->dc_pin_);
  LOG_PIN("  WR Pin: ", this->wr_pin_);
  LOG_PIN("  RD Pin: ", this->rd_pin_);
  ESP_LOGCONFIG(TAG, "  Data Pins: D0-D7");
  ESP_LOGCONFIG(TAG, "  Width: %d, Height: %d", this->width_, this->height_);
  ESP_LOGCONFIG(TAG, "  Rotation: %d", this->rotation_);
}

float ILI9341ParallelDisplay::get_setup_priority() const { return setup_priority::PROCESSOR; }

void ILI9341ParallelDisplay::update() {
  this->do_update_();
  this->write_display_data_();
}

void ILI9341ParallelDisplay::write_display_data_() {
  uint16_t x1 = 0;
  uint16_t x2 = this->get_width_internal() - 1;
  uint16_t y1 = 0;
  uint16_t y2 = this->get_height_internal() - 1;

  this->send_command_(ILI9XXX_CASET);
  this->send_data_(x1 >> 8);
  this->send_data_(x1);
  this->send_data_(x2 >> 8);
  this->send_data_(x2);

  this->send_command_(ILI9XXX_PASET);
  this->send_data_(y1 >> 8);
  this->send_data_(y1);
  this->send_data_(y2 >> 8);
  this->send_data_(y2);

  this->send_command_(ILI9XXX_RAMWR);

  for (int y = 0; y < this->get_height_internal(); y++) {
    for (int x = 0; x < this->get_width_internal(); x++) {
      auto color = this->get_pixel_color_(x, y);
      uint16_t rgb565 = color_to_rgb565(color);
      this->send_data_(rgb565 >> 8);
      this->send_data_(rgb565);
    }
  }
}

void ILI9341ParallelDisplay::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= this->get_width_internal() || x < 0 || y >= this->get_height_internal() || y < 0)
    return;

  uint16_t rgb565 = color_to_rgb565(color);

  this->send_command_(ILI9XXX_CASET);
  this->send_data_(x >> 8);
  this->send_data_(x);
  this->send_data_(x >> 8);
  this->send_data_(x);

  this->send_command_(ILI9XXX_PASET);
  this->send_data_(y >> 8);
  this->send_data_(y);
  this->send_data_(y >> 8);
  this->send_data_(y);

  this->send_command_(ILI9XXX_RAMWR);
  this->send_data_(rgb565 >> 8);
  this->send_data_(rgb565);
}

void ILI9341ParallelDisplay::init_lcd_() {
  this->reset_();
  
  // Inicialización del LCD
  static const uint8_t init_cmd[] = {
    ILI9XXX_SWRESET, 0x80,  // Software reset, 0x80 means delay is needed
    ILI9XXX_PWCTRB, 3, 0x00, 0xC1, 0x30,
    ILI9XXX_PWRONCTRL, 4, 0x64, 0x03, 0x12, 0x81,
    ILI9XXX_DTCTRLA, 3, 0x85, 0x00, 0x78,
    ILI9XXX_PWCTRSEQ, 4, 0x39, 0x2C, 0x00, 0x34,
    ILI9XXX_PUMPCTRL, 1, 0x20,
    ILI9XXX_DTCTRLB, 2, 0x00, 0x00,
    ILI9XXX_PWCTRL1, 1, 0x23,
    ILI9XXX_PWCTRL2, 1, 0x10,
    ILI9XXX_VMCTRL1, 2, 0x3e, 0x28,
    ILI9XXX_VMCTRL2, 1, 0x86,
    ILI9XXX_MADCTL, 1, 0x48,
    ILI9XXX_PIXFMT, 1, 0x55,
    ILI9XXX_FRMCTR1, 2, 0x00, 0x18,
    ILI9XXX_DFUNCTR, 3, 0x08, 0x82, 0x27,
    ILI9XXX_ENGMCTRL, 1, 0x00,
    ILI9XXX_GAMMASET, 1, 0x01,
    ILI9XXX_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    ILI9XXX_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    ILI9XXX_SLPOUT, 0x80,  // Exit sleep mode, 0x80 means delay is needed
    ILI9XXX_DISPON, 0x80,  // Display on, 0x80 means delay is needed
    0x00  // End of list
  };

  for (uint8_t i = 0; init_cmd[i] != 0x00; ) {
    uint8_t cmd = init_cmd[i++];
    uint8_t num_args = init_cmd[i++];
    bool delay_after = num_args & 0x80;
    num_args &= 0x7F;
    
    this->send_command_(cmd);
    for (uint8_t j = 0; j < num_args; j++) {
      this->send_data_(init_cmd[i++]);
    }
    
    if (delay_after) {
      delay(120);
    }
  }
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

void ILI9341ParallelDisplay::send_command_(uint8_t cmd) {
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->digital_write(false);
  }
  this->dc_pin_->digital_write(false);
  this->write_byte(cmd);
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->digital_write(true);
  }
}

void ILI9341ParallelDisplay::send_data_(uint8_t data) {
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->digital_write(false);
  }
  this->dc_pin_->digital_write(true);
  this->write_byte(data);
  if (this->cs_pin_ != nullptr) {
    this->cs_pin_->digital_write(true);
  }
}

void ILI9341ParallelDisplay::write_byte(uint8_t value) {
  for (int i = 0; i < 8; i++) {
    this->data_pins_[i]->digital_write((value >> i) & 0x01);
  }
  this->wr_pin_->digital_write(false);
  delayMicroseconds(1);  // Asegura el tiempo mínimo de pulso WR
  this->wr_pin_->digital_write(true);
  delayMicroseconds(1);  // Tiempo de espera entre bytes
}

void ILI9341ParallelDisplay::set_rotation(int rotation) {
  this->rotation_ = rotation % 4;
  switch (this->rotation_) {
    case 0:
      this->send_command_(ILI9XXX_MADCTL);
      this->send_data_(MADCTL_MX | MADCTL_BGR);
      break;
    case 1:
      this->send_command_(ILI9XXX_MADCTL);
      this->send_data_(MADCTL_MV | MADCTL_BGR);
      break;
    case 2:
      this->send_command_(ILI9XXX_MADCTL);
      this->send_data_(MADCTL_MY | MADCTL_BGR);
      break;
    case 3:
      this->send_command_(ILI9XXX_MADCTL);
      this->send_data_(MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR);
      break;
  }
}

Color ILI9341ParallelDisplay::get_pixel_color_(int x, int y) const {
  return Color(this->buffer_[(x + y * this->width_) * 3],
               this->buffer_[(x + y * this->width_) * 3 + 1],
               this->buffer_[(x + y * this->width_) * 3 + 2]);
}

uint16_t ILI9341ParallelDisplay::color_to_rgb565(Color color) {
  return ((color.r & 0xF8) << 8) | ((color.g & 0xFC) << 3) | (color.b >> 3);
}

void ILI9341ParallelDisplay::fill(Color color) {
  uint16_t rgb565 = color_to_rgb565(color);
  for (int y = 0; y < this->get_height_internal(); y++) {
    for (int x = 0; x < this->get_width_internal(); x++) {
      this->draw_absolute_pixel_internal(x, y, color);
    }
  }
}

int ILI9341ParallelDisplay::get_width_internal() { return this->width_; }
int ILI9341ParallelDisplay::get_height_internal() { return this->height_; }

void ILI9341ParallelDisplay::set_data_pin(uint8_t index, GPIOPin *pin) {
  if (index < 8) {
    this->data_pins_[index] = pin;
  } else {
    ESP_LOGE(TAG, "Invalid data pin index: %d", index);
  }
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

}  // namespace ili9xxx
}  // namespace esphome
