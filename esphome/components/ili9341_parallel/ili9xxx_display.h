#pragma once
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"
#include "ili9xxx_defines.h"

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
  void fill(Color color);
  void dump_config();

  Color get_pixel_color(int x, int y);

  // Implementación de funciones virtuales puras
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_OTHER; }
  int get_height_internal() override { return this->height_; }
  int get_width_internal() override { return this->width_; }

 protected:
  void init_pins_();
  void init_lcd_();
  void write_display_();
  void do_update_();
  void write_command_(uint8_t cmd);
  void write_data_(uint8_t data);
  void set_addr_window_(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  // Declaraciones de funciones que faltaban
  void write_color_(Color color);
  void write_byte_(uint8_t data);

  // Declaraciones de funciones set*
  void set_data_pin(int index, GPIOPin *pin);
  void set_dc_pin(GPIOPin *pin);
  void set_wr_pin(GPIOPin *pin);
  void set_rd_pin(GPIOPin *pin);
  void set_reset_pin(GPIOPin *pin);
  void set_cs_pin(GPIOPin *pin);

 private:
  int width_;
  int height_;
  GPIOPin *data_pins_[8];
  GPIOPin *dc_pin_;
  GPIOPin *wr_pin_;
  GPIOPin *rd_pin_;
  GPIOPin *reset_pin_;
  GPIOPin *cs_pin_;

  Color get_buffer_pixel(int x, int y);
};

}  // namespace ili9xxx
}  // namespace esphome
