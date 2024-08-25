#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace ili9341_parallel {

class ILI9341ParallelDisplay : public Component, public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  void fill(Color color) override;
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

  void set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3, GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7) {
    data_pins_[0] = d0;
    data_pins_[1] = d1;
    data_pins_[2] = d2;
    data_pins_[3] = d3;
    data_pins_[4] = d4;
    data_pins_[5] = d5;
    data_pins_[6] = d6;
    data_pins_[7] = d7;
  }
  void set_reset_pin(GPIOPin *reset_pin) { reset_pin_ = reset_pin; }
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  void set_wr_pin(GPIOPin *wr_pin) { wr_pin_ = wr_pin; }
  void set_rd_pin(GPIOPin *rd_pin) { rd_pin_ = rd_pin; }
  void set_cs_pin(GPIOPin *cs_pin) { cs_pin_ = cs_pin; }

 protected:
  GPIOPin *data_pins_[8];
  GPIOPin *reset_pin_;
  GPIOPin *dc_pin_;
  GPIOPin *wr_pin_;
  GPIOPin *rd_pin_;
  GPIOPin *cs_pin_;
};

}  // namespace ili9341_parallel
}  // namespace esphome
