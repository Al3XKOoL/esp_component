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
  ILI9341ParallelDisplay();
  ~ILI9341ParallelDisplay();

  void setup() override;
  void update() override;
  void init(int w, int h);
  void set_rotation(uint8_t rotation);
  void set_width(uint16_t width);
  void set_height(uint16_t height);
  void set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3,
                     GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7);
  void set_data_pin(uint8_t index, GPIOPin *pin);
  Color get_pixel_color(int x, int y);

 protected:
  void init_pins_();
  void init_lcd_();
  void hard_reset_();
  void write_display_();
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

 private:
  // ... (otros miembros privados)
};

}  // namespace ili9xxx
}  // namespace esphome
