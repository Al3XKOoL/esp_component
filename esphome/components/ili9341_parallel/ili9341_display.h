#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/gpio/gpio.h"
#include "ili9341_defines.h"

namespace esphome {
namespace ili9341_parallel {

class ILI9341ParallelDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;
  
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  void set_wr_pin(GPIOPin *wr_pin) { wr_pin_ = wr_pin; }
  void set_rd_pin(GPIOPin *rd_pin) { rd_pin_ = rd_pin; }
  void set_cs_pin(GPIOPin *cs_pin) { cs_pin_ = dc_pin; }
  void set_reset_pin(GPIOPin *reset_pin) { reset_pin_ = reset_pin; }
  void set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3,
                     GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7) {
    data_pins_[0] = d0;
    data_pins_[1] = d1;
    data_pins_[2] = d2;
    data_pins_[3] = d3;
    data_pins_[4] = d4;
    data_pins_[5] = d5;
    data_pins_[6] = d6;
    data_pins_[7] = d7;
  }
  
  void set_dimensions(uint16_t width, uint16_t height) {
    this->width_ = width;
    this->height_ = height;
  }
  
  void set_rotation(display::DisplayRotation rotation);
  
  void command(uint8_t value);
  void data(uint8_t value);
  void send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes);

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  
  void init_pins_();
  void init_lcd_();
  void reset_();
  void write_byte(uint8_t data);
  void set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void display_();
  
  uint32_t get_buffer_length_();
  
  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();
  
  GPIOPin *dc_pin_;
  GPIOPin *wr_pin_;
  GPIOPin *rd_pin_;
  GPIOPin *cs_pin_;
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *data_pins_[8];
  
  uint16_t width_{240};
  uint16_t height_{320};
  display::DisplayRotation rotation_{display::DISPLAY_ROTATION_0_DEGREES};
};

} // namespace ili9341_parallel
} // namespace esphome
