#pragma once
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"
#include "ili9xxx_defines.h"
#include <driver/gpio.h>

namespace esphome {
namespace ili9xxx {

class ILI9341ParallelDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  void fill(Color color) override;
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

 protected:
  void write_command_(uint8_t cmd);
  void write_data_(uint8_t data);
  void write_byte_(uint8_t data);
  void write_color_(Color color);
  void set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

  GPIOPin *dc_pin_;
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *wr_pin_;
  GPIOPin *rd_pin_;
  GPIOPin *cs_pin_{nullptr};
  GPIOPin *data_pins_[8];
  uint16_t width_{240};
  uint16_t height_{320};
  uint8_t rotation_{0};
};

}  // namespace ili9xxx
}  // namespace esphome
