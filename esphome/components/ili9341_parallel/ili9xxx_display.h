#pragma once
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"  // Añade esta línea
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

  // Implementa estos métodos
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }
  int get_width_internal() const override { return this->width_; }
  int get_height_internal() override { return this->height_; }
  
  void set_width(int width) { this->width_ = width; }
  void set_height(int height) { this->height_ = height; }
  void set_rotation(int rotation) { set_rotation(static_cast<display::DisplayRotation>(rotation)); }
  void set_rotation(display::DisplayRotation rotation);
  void set_data_pins(GPIOPin *d0, GPIOPin *d1, GPIOPin *d2, GPIOPin *d3, GPIOPin *d4, GPIOPin *d5, GPIOPin *d6, GPIOPin *d7);
  void set_wr_pin(GPIOPin *wr_pin) { this->wr_pin_ = wr_pin; }
  void set_rd_pin(GPIOPin *rd_pin) { this->rd_pin_ = rd_pin; }
  void set_dc_pin(GPIOPin *dc_pin) { this->dc_pin_ = dc_pin; }
  void set_reset_pin(GPIOPin *reset_pin) { this->reset_pin_ = reset_pin; }
  void set_cs_pin(GPIOPin *cs_pin) { this->cs_pin_ = cs_pin; }

  using display::DisplayBuffer::set_update_interval;
  using esphome::PollingComponent::set_component_source;

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void write_display_data_();
  void init_lcd_();
  void reset_();
  void send_command_(uint8_t cmd);
  void send_data_(uint8_t data);
  void write_byte(uint8_t value);

  int width_{240};
  int height_{320};
  GPIOPin *data_pins_[8];
  GPIOPin *dc_pin_;
  GPIOPin *wr_pin_;
  GPIOPin *rd_pin_;
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *cs_pin_{nullptr};
  display::DisplayRotation rotation_{display::DISPLAY_ROTATION_0_DEGREES};

  static uint16_t color_to_rgb565(Color color);
  Color get_pixel_color_(int x, int y) const;
};

}  // namespace ili9xxx
}  // namespace esphome
