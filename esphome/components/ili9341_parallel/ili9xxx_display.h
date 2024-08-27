#pragma once
#include "esphome/core/color.h"
#include "esphome/components/display/display_buffer.h"
#include "ili9xxx_defines.h"

#define BUFFER_SIZE 32  // Ajusta este valor según la memoria disponible

namespace esphome {
namespace ili9xxx {

class ILI9341ParallelDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;
  void fill(Color color) override;

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
  void set_rotation(display::DisplayRotation rotation);

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;

  void write_display_data_();
  void init_lcd_();
  void reset_();
  void send_command_(uint8_t cmd);
  void send_data_(uint8_t data);
  void write_byte(uint8_t value);

  static uint16_t color_to_rgb565(Color color);

  GPIOPin *data_pins_[8]{};
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *wr_pin_{nullptr};
  GPIOPin *rd_pin_{nullptr};
  GPIOPin *cs_pin_{nullptr};
  uint16_t width_{320};
  uint16_t height_{240};
  display::DisplayRotation rotation_{display::DISPLAY_ROTATION_0_DEGREES};
};

}  // namespace ili9xxx
}  // namespace esphome
