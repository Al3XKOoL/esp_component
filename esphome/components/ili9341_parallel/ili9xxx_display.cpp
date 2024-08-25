#include "ili9xxx_display.h"
#include "esphome/core/application.h"
#include "esphome/core/hal.h"
#include "esphome/core/helpers.h"
#include "esphome/core/log.h"

namespace esphome {
namespace ili9xxx {

static const uint16_t MAX_BLOCK_SIZE = 4092;  // Max size of continuous transfer

// Store a 16-bit value in a buffer, big endian
static inline void put16_be(uint8_t *buf, uint16_t value) {
  buf[0] = value >> 8;
  buf[1] = value;
}

void ILI9XXXDisplay::set_madctl() {
  uint8_t mad = this->color_order_ == display::COLOR_ORDER_BGR ? MADCTL_BGR : MADCTL_RGB;
  if (this->swap_xy_)
    mad |= MADCTL_MV;
  if (this->mirror_x_)
    mad |= MADCTL_MX;
  if (this->mirror_y_)
    mad |= MADCTL_MY;
  this->command(ILI9XXX_MADCTL);
  this->data(mad);
  ESP_LOGD(TAG, "Wrote MADCTL 0x%02X", mad);
}

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
  if (this->processing_update_) {  
    this->need_update_ = true;
    return;
  }
  this->processing_update_ = true;  
  do {
    this->need_update_ = false;
    this->do_update_();
  } while (this->need_update_);
  this->processing_update_ = false;  
  this->display_();
}

void ILI9XXXDisplay::display_() {
  if ((this->x_high_ < this->x_low_) || (this->y_high_ < this->y_low_)) {
    return;
  }

  size_t w = this->x_high_ - this->x_low_ + 1;
  size_t h = this->y_high_ - this->y_low_ + 1;

  auto now = millis();
  if (this->buffer_color_mode_ == BITS_16 && !this->is_18bitdisplay_) {
    ESP_LOGD(TAG, "Doing single write of %zu bytes", this->width_ * h * 2);
    set_addr_window_(0, this->y_low_, this->width_ - 1, this->y_high_);
    this->write_array(this->buffer_ + this->y_low_ * this->width_ * 2, h * this->width_ * 2);
  } else {
    ESP_LOGD(TAG, "Doing multiple writes");
    uint8_t transfer_buffer[MAX_BLOCK_SIZE];
    size_t rem = h * w;  
    set_addr_window_(this->x_low_, this->y_low_, this->x_high_, this->y_high_);
    size_t idx = 0;    
    size_t pixel = 0;  
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
        default:  
          color_val = (this->buffer_[pos * 2] << 8) + this->buffer_[pos * 2 + 1];
          pos++;
          break;
      }
      if (this->is_18bitdisplay_) {
        transfer_buffer[idx++] = (uint8_t) ((color_val & 0xF800) >> 8);  
        transfer_buffer[idx++] = (uint8_t) ((color_val & 0x7E0) >> 3);   
        transfer_buffer[idx++] = (uint8_t) (color_val << 3);             
      } else {
        put16_be(transfer_buffer + idx, color_val);
        idx += 2;
      }
      if (idx == sizeof(transfer_buffer)) {
        this->write_array(transfer_buffer, idx);
        idx = 0;
        App.feed_wdt();
      }
      if (++pixel == w) {
        pixel = 0;
        pos += this->width_ - w;
      }
    }
    if (idx != 0) {
      this->write_array(transfer_buffer, idx);
    }
  }
  this->end_data_();
  ESP_LOGD(TAG, "Data write took %dms", (unsigned) (millis() - now));
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
  if (bitness == display::BITS_8 || order != this->color_order_ ||
      (x_offset != 0 || y_offset != 0 || x_pad != 0)) {
    this->mark_failed();
    return;
  }
  if (x_start < 0 || y_start < 0 || x_start >= this->width_ || y_start >= this->height_) {
    ESP_LOGE(TAG, "draw_pixels_at out of bounds: %d,%d w:%d h:%d", x_start, y_start, w, h);
    this->mark_failed();
    return;
  }
  if (x_start + w > this->width_)
    w = this->width_ - x_start;
  if (y_start + h > this->height_)
    h = this->height_ - y_start;
  if (w <= 0 || h <= 0)
    return;
  this->start_data_();
  this->set_addr_window_(x_start, y_start, x_start + w - 1, y_start + h - 1);
  size_t idx = 0;
  uint8_t pixel[2];
  for (int y = y_start; y < y_start + h; y++) {
    for (int x = x_start; x < x_start + w; x++) {
      if (bitness == display::BITS_16) {
        pixel[0] = ptr[idx++];
        pixel[1] = ptr[idx++];
        this->write_array(pixel, 2);
      } else {
        uint8_t color = ptr[idx++];
        uint16_t color_val = display::ColorUtil::color_to_565(
            display::ColorUtil::index8_to_color_palette888(color, this->palette_));
        pixel[0] = color_val >> 8;
        pixel[1] = color_val;
        this->write_array(pixel, 2);
      }
    }
  }
  this->end_data_();
}

}  // namespace ili9xxx
}  // namespace esphome
