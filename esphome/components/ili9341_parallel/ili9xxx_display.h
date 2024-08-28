#pragma once
#include "esphome/core/color.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"

// Definiciones de comandos ILI9XXX
#define ILI9XXX_NOP 0x00
#define ILI9XXX_SWRESET 0x01
#define ILI9XXX_RDDID 0x04
#define ILI9XXX_RDDST 0x09
#define ILI9XXX_SLPIN 0x10
#define ILI9XXX_SLPOUT 0x11
#define ILI9XXX_PTLON 0x12
#define ILI9XXX_NORON 0x13
#define ILI9XXX_RDMODE 0x0A
#define ILI9XXX_RDMADCTL 0x0B
#define ILI9XXX_RDPIXFMT 0x0C
#define ILI9XXX_RDIMGFMT 0x0D
#define ILI9XXX_RDSELFDIAG 0x0F
#define ILI9XXX_INVOFF 0x20
#define ILI9XXX_INVON 0x21
#define ILI9XXX_GAMMASET 0x26
#define ILI9XXX_DISPOFF 0x28
#define ILI9XXX_DISPON 0x29
#define ILI9XXX_CASET 0x2A
#define ILI9XXX_PASET 0x2B
#define ILI9XXX_RAMWR 0x2C
#define ILI9XXX_RAMRD 0x2E
#define ILI9XXX_PTLAR 0x30
#define ILI9XXX_MADCTL 0x36
#define ILI9XXX_PIXFMT 0x3A
#define ILI9XXX_FRMCTR1 0xB1
#define ILI9XXX_FRMCTR2 0xB2
#define ILI9XXX_FRMCTR3 0xB3
#define ILI9XXX_INVCTR 0xB4
#define ILI9XXX_DFUNCTR 0xB6
#define ILI9XXX_PWCTR1 0xC0
#define ILI9XXX_PWCTR2 0xC1
#define ILI9XXX_PWCTR3 0xC2
#define ILI9XXX_PWCTR4 0xC3
#define ILI9XXX_PWCTR5 0xC4
#define ILI9XXX_VMCTR1 0xC5
#define ILI9XXX_VMCTR2 0xC7
#define ILI9XXX_RDID1 0xDA
#define ILI9XXX_RDID2 0xDB
#define ILI9XXX_RDID3 0xDC
#define ILI9XXX_RDID4 0xDD
#define ILI9XXX_GMCTRP1 0xE0
#define ILI9XXX_GMCTRN1 0xE1

// Bits para MADCTL
#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

namespace esphome {
namespace ili9xxx {

class ILI9341ParallelDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;
  void fill(Color color) override;
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

  void set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3,
                     GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7);
  void set_data_pin(uint8_t index, GPIOPin *pin);
  void set_dc_pin(GPIOPin *dc_pin) { this->dc_pin_ = dc_pin; }
  void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }
  void set_cs_pin(GPIOPin *cs_pin) { this->cs_pin_ = cs_pin; }
  void set_wr_pin(GPIOPin *wr_pin) { this->wr_pin_ = wr_pin; }
  void set_rd_pin(GPIOPin *rd_pin) { this->rd_pin_ = rd_pin; }

  void set_width(uint16_t width) { this->width_ = width; }
  void set_height(uint16_t height) { this->height_ = height; }
  void set_rotation(int rotation);

 protected:
  static uint16_t color_to_rgb565(Color color);
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;

  void write_display_data_();
  void init_lcd_();
  void reset_();
  void send_command_(uint8_t cmd);
  void send_data_(uint8_t data);
  void write_byte(uint8_t value);

  Color get_pixel_color_(int x, int y) const;

  GPIOPin *data_pins_[8]{};
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *wr_pin_{nullptr};
  GPIOPin *rd_pin_{nullptr};
  GPIOPin *cs_pin_{nullptr};
  uint16_t width_{320};
  uint16_t height_{240};
  int rotation_{0};
};

}  // namespace ili9xxx
}  // namespace esphome
