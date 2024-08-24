// ili9xxx_display.cpp
#include "ili9xxx_display.h"
#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ili9xxx {

static const uint16_t PARALLEL_SETUP_US = 100; // estimated fixed overhead in microseconds for a parallel write
static const uint16_t PARALLEL_MAX_BLOCK_SIZE = 4092; // Max size of continuous parallel transfer

void ILI9XXXDisplay::setup() {
  ESP_LOGD(TAG, "Setting up ILI9xxx");

  this->setup_pins_();
  this->init_lcd_(this->init_sequence_);
  this->init_lcd_(this->extra_init_sequence_.data());

  if (this->is_parallel_) {
    this->command(ILI9XXX_INTERFACE_CONTROL);
    this->data(ILI9XXX_INTERFACE_8BIT_PARALLEL);
  }

  switch (this->pixel_mode_) {
    case PIXEL_MODE_16:
      if (this->is_18bitdisplay_) {
        this->command(ILI9XXX_PIXFMT);
        this->data(0x55);
        this->is_18bitdisplay_ = false;
      }
      break;
    case PIXEL_MODE_18:
      if (!this->is_18bitdisplay_) {
        this->command(ILI9XXX_PIXFMT);
        this->data(0x66);
        this->is_18bitdisplay_ = true;
      }
      break;
    default:
      break;
  }

  this->set_madctl();
  this->command(this->pre_invertcolors_ ? ILI9XXX_INVON : ILI9XXX_INVOFF);
  this->x_low_ = this->width_;
  this->y_low_ = this->height_;
  this->x_high_ = 0;
  this->y_high_ = 0;
}

void ILI9XXXDisplay::alloc_buffer_() {
  if (this->buffer_color_mode_ == BITS_16) {
    this->init_internal_(this->get_buffer_length_() * 2);
    if (this->buffer_ != nullptr) {
      return;
    }
    this->buffer_color_mode_ = BITS_8;
  }
  this->init_internal_(this->get_buffer_length_());
  if (this->buffer_ == nullptr) {
    this->mark_failed();
  }
}

void ILI9XXXDisplay::update() {
  if (this->prossing_update_) {
    this->need_update_ = true;
    return;
  }
  this->prossing_update_ = true;
  do {
    this->need_update_ = false;
    this->do_update_();
  } while (this->need_update_);
  this->prossing_update_ = false;
  this->display_();
}

void ILI9XXXDisplay::display_() {
  // check if something was displayed
  if ((this->x_high_ < this->x_low_) || (this->y_high_ < this->y_low_)) {
    return;
  }

  // we will only update the changed rows to the display
  size_t const w = this->x_high_ - this->x_low_ + 1;
  size_t const h = this->y_high_ - this->y_low_ + 1;

  size_t mhz = this->data_rate_ / 1000000;
  // estimate time for a single write
  size_t sw_time = this->width_ * h * 16 / mhz + this->width_ * h * 2 / PARALLEL_MAX_BLOCK_SIZE * PARALLEL_SETUP_US * 2;
  // estimate time for multiple writes
  size_t mw_time = (w * h * 16) / mhz + w * h * 2 / PARALLEL_MAX_BLOCK_SIZE * PARALLEL_SETUP_US;
  ESP_LOGV(TAG,
           "Start display(xlow:%d, ylow:%d, xhigh:%d, yhigh:%d, width:%d, "
           "height:%zu, mode=%d, 18bit=%d, sw_time=%zuus, mw_time=%zuus)",
           this->x_low_, this->y_low_, this->x_high_, this->y_high_, w, h, this->buffer_color_mode_,
           this->is_18bitdisplay_, sw_time, mw_time);
  auto now = millis();
  if (this->buffer_color_mode_ == BITS_16 && !this->is_18bitdisplay_ && sw_time < mw_time) {
    // 16 bit mode maps directly to display format
    ESP_LOGV(TAG, "Doing single write of %zu bytes", this->width_ * h * 2);
    set_addr_window_(0, this->y_low_, this->width_ - 1, this->y_high_);
    this->write_array(this->buffer_ + this->y_low_ * this->width_ * 2, h * this->width_ * 2);
  } else {
    ESP_LOGV(TAG, "Doing multiple write");
    uint8_t transfer_buffer[PARALLEL_MAX_BLOCK_SIZE];
    size_t rem = h * w; // remaining number of pixels to write
    set_addr_window_(this->x_low_, this->y_low_, this->x_high_, this->y_high_);
    size_t idx = 0; // index into transfer_buffer
    size_t pixel = 0; // pixel number offset
    size_t pos = this->y_low_ * this->width_ + this->x_low_;
    while (rem-- != 0) {
      uint16_t color_val;
      switch (this->buffer_color_mode_) {
        case BITS_8:
          color_val = display::ColorUtil::color_to_565(display::ColorUtil::rgb332_to_color(this->buffer_[pos++]));
          break;
        case BITS_8_INDEXED:
          color_val = display::ColorUtil::color_to_565(
              display::ColorUtil::index8_to_color_palette888(this->buffer_[pos++], this->palette_));
          break;
        default: // case BITS_16:
          color_val = (this->buffer_[pos * 2] << 8) + this->buffer_[pos * 2 + 1];
          pos++;
          break;
      }
      if (this->is_18bitdisplay_) {
        transfer_buffer[idx++] = (uint8_t)((color_val & 0xF800) >> 8); // Blue
        transfer_buffer[idx++] = (uint8_t)((color_val & 0x7E0) >> 3); // Green
        transfer_buffer[idx++] = (uint8_t)(color_val << 3); // Red
      } else {
        transfer_buffer[idx++] = (color_val >> 8);
        transfer_buffer[idx++] = (color_val & 0xFF);
      }
      if (idx == sizeof(transfer_buffer)) {
        this->write_array(transfer_buffer, idx);
        idx = 0;
        App.feed_wdt();
      }
      // end of line? Skip to the next.
      if (++pixel == w) {
        pixel = 0;
        pos += this->width_ - w;
      }
    }
    // flush any balance.
    if (idx != 0) {
      this->write_array(transfer_buffer, idx);
    }
  }
  this->end_data_();
  ESP_LOGV(TAG, "Data write took %dms", (unsigned)(millis() - now));
  // invalidate watermarks
  this->x_low_ = this->width_;
  this->y_low_ = this->height_;
  this->x_high_ = 0;
  this->y_high_ = 0;
}

void ILI9XXXDisplay::draw_pixels_at(int x_start, int y_start, int w, int h, const uint8_t *ptr,
                                    display::ColorOrder order, display::ColorBitness bitness, bool big_endian,
                                    int x_offset, int y_offset, int x_pad) {
  if (w <= 0 || h <= 0)
    return;
  // if color mapping or software rotation is required, hand this off to the parent implementation. This will
  // do color conversion pixel-by-pixel into the buffer and draw it later. If this is happening the user has not
  // configured the renderer well.
  if (this->rotation_ != display::DISPLAY_ROTATION_0_DEGREES || bitness != display::COLOR_BITNESS_565 || !big_endian) {
    return display::Display::draw_pixels_at(x_start, y_start, w, h, ptr, order, bitness, big_endian, x_offset, y_offset,
                                            x_pad);
  }
  this->set_addr_window_(x_start, y_start, x_start + w - 1, y_start + h - 1);
  // x_ and y_offset are offsets into the source buffer, unrelated to our own offsets into the display.
  auto stride = x_offset + w + x_pad;
  if (!this->is_18bitdisplay_) {
    if (x_offset == 0 && x_pad == 0 && y_offset == 0) {
      // we could deal here with a non-zero y_offset, but if x_offset is zero, y_offset probably will be so don't bother
      this->write_array(ptr, w * h * 2);
    } else {
      for (size_t y = 0; y != h; y++) {
        this->write_array(ptr + (y + y_offset) * stride + x_offset, w * 2);
      }
    }
  } else {
    // 18 bit mode
    uint8_t transfer_buffer[PARALLEL_MAX_BLOCK_SIZE * 4];
    ESP_LOGV(TAG, "Doing multiple write");
    size_t rem = h * w; // remaining number of pixels to write
    size_t idx = 0; // index into transfer_buffer
    size_t pixel = 0; // pixel number offset
    ptr += (y_offset * stride + x_offset) * 2;
    while (rem-- != 0) {
      uint8_t hi_byte = *ptr++;
      uint8_t lo_byte = *ptr++;
      transfer_buffer[idx++] = hi_byte & 0xF8; // Blue
      transfer_buffer[idx++] = ((hi_byte << 5) | (lo_byte) >> 5); // Green
      transfer_buffer[idx++] = lo_byte << 3; // Red
      if (idx == sizeof(transfer_buffer)) {
        this->write_array(transfer_buffer, idx);
        idx = 0;
        App.feed_wdt();
      }
      // end of line? Skip to the next.
      if (++pixel == w) {
        pixel = 0;
        ptr += (x_pad + x_offset) * 2;
      }
    }
    // flush any balance.
    if (idx != 0) {
      this->write_array(transfer_buffer, idx);
    }
  }
  this->end_data_();
}

void ILI9XXXDisplay::command(uint8_t value) {
  if (this->interface_mode_ == PARALLEL_8BIT_MODE) {
    this->wr_pin_->digital_write(false); // Set W/R pin to low for write
    for (int i = 0; i < 8; i++) {
      this->data_pins_[i]->digital_write((value >> i) & 1);
    }
    this->wr_pin_->digital_write(true); // Pulse W/R pin high to latch data
    this->wr_pin_->digital_write(false); // Set W/R pin to low for command mode
    this->data_pins_->digital_write(ILI9XXX_CMD_MODE); // Set data pins to command mode
    this->wr_pin_->digital_write(true); // Pulse W/R pin high to latch command mode
  }
}

void ILI9XXXDisplay::data(uint8_t value) {
  if (this->interface_mode_ == PARALLEL_8BIT_MODE) {
    this->wr_pin_->digital_write(false); // Set W/R pin to low for write
    for (int i = 0; i < 8; i++) {
      this->data_pins_[i]->digital_write((value >> i) & 1);
    }
    this->wr_pin_->digital_write(true); // Pulse W/R pin high to latch data
    this->wr_pin_->digital_write(false); // Set W/R pin to low for data mode
    this->data_pins_->digital_write(ILI9XXX_DATA_MODE); // Set data pins to data mode
    this->wr_pin_->digital_write(true); // Pulse W/R pin high to latch data mode
  }
}

void ILI9XXXDisplay::send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes) {
  if (this->interface_mode_ == PARALLEL_8BIT_MODE) {
    this->command(command_byte);
    for (int i = 0; i < num_data_bytes; i++) {
      this->data(data_bytes[i]);
    }
  }
}

void ILI9XXXDisplay::init_lcd_(const uint8_t *addr) {
  if (addr == nullptr)
    return;
  uint8_t cmd, x, num_args;
  while ((cmd = *addr++) != 0) {
    x = *addr++;
    if (x == ILI9XXX_DELAY_FLAG) {
      cmd &= 0x7F;
      ESP_LOGV(TAG, "Delay %dms", cmd);
      delay(cmd);
    } else {
      num_args = x & 0x7F;
      ESP_LOGV(TAG, "Command %02X, length %d, bits %02X", cmd, num_args, *addr);
      this->send_command(cmd, addr, num_args);
      addr += num_args;
      if (x & 0x80) {
        ESP_LOGV(TAG, "Delay 150ms");
        delay(150); // NOLINT
      }
    }
  }
}

void ILI9XXXDisplay::set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
  x1 += this->offset_x_;
  x2 += this->offset_x_;
  y1 += this->offset_y_;
  y2 += this->offset_y_;
  this->command(ILI9XXX_CASET);
  this->data(x1 >> 8);
  this->data(x1 & 0xFF);
  this->data(x2 >> 8);
  this->data(x2 & 0xFF);
  this->command(ILI9XXX_PASET); // Page address set
  this->data(y1 >> 8);
  this->data(y1 & 0xFF);
  this->data(y2 >> 8);
  this->data(y2 & 0xFF);
  this->command(ILI9XXX_RAMWR); // Write to RAM
  this->start_data_();
}

void ILI9XXXDisplay::invert_colors(bool invert) {
  this->pre_invertcolors_ = invert;
  if (is_ready()) {
    this->command(invert ? ILI9XXX_INVON : ILI9XXX_INVOFF);
  }
}

int ILI9XXXDisplay::get_width_internal() { return this->width_; }
int ILI9XXXDisplay::get_height_internal() { return this->height_; }

void ILI9XXXDisplay::reset_() {
  if (this->reset_pin_ != nullptr) {
    this->reset_pin_->digital_write(false);
    delay(20);
    this->reset_pin_->digital_write(true);
    delay(20);
  }
}

uint32_t ILI9XXXDisplay::get_buffer_length_() { return this->get_width_internal() * this->get_height_internal(); }

void ILI9XXXDisplay::write_array(const uint8_t *data, size_t len) {
  for (size_t i = 0; i < len; i++) {
    this->data(data[i]);
  }
}

void ILI9XXXDisplay::write_byte(uint8_t value) {
  this->data(value);
}

} // namespace ili9xxx
} // namespace esphome
