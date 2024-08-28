#pragma once
#include "esphome/core/color.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"
#include "ili9xxx_defines.h"

namespace esphome {
namespace ili9xxx {

class ILI9341ParallelDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  void update() override;
  void fill(Color color) override;
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  void set_reset_pin(GPIOPin *reset_pin) { reset_pin_ = reset_pin; }
  void set_wr_pin(GPIOPin *wr_pin) { wr_pin_ = wr_pin; }
  void set_rd_pin(GPIOPin *rd_pin) { rd_pin_ = rd_pin; }
  void set_cs_pin(GPIOPin *cs_pin) { cs_pin_ = cs_pin; }
  void set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3,
                     GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7);
  void set_data_pin(uint8_t index, GPIOPin *pin);
  void set_width(uint16_t width) { width_ = width; }
  void set_height(uint16_t height) { height_ = height; }
  void set_rotation(uint8_t rotation) { rotation_ = rotation; }

  void print(int x, int y, display::Font *font, Color color, const char *text) override {
    this->display_buffer_->print(x, y, font, color, text);
  }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override { return width_; }
  int get_height_internal() override { return height_; }

  void init_lcd_();
  void send_command_(uint8_t cmd);
  void send_data_(uint8_t data);
  void write_byte_(uint8_t value);

  GPIOPin *data_pins_[8]{};
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *wr_pin_{nullptr};
  GPIOPin *rd_pin_{nullptr};
  GPIOPin *cs_pin_{nullptr};
  uint16_t width_{320};
  uint16_t height_{240};
  uint8_t rotation_{0};
};

}  // namespace ili9xxx
}  // namespace esphome
