#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace ili9xxx {

class ILI9341ParallelDisplay : public Component, public display::DisplayBuffer {
public:
    void setup() override;
    void dump_config() override;
    float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  
  void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
  void set_wr_pin(GPIOPin *pin) { wr_pin_ = pin; }
  void set_rd_pin(GPIOPin *pin) { rd_pin_ = pin; }
  void set_data_pin(uint8_t index, GPIOPin *pin) { data_pins_[index] = pin; }
  void set_dimensions(uint16_t width, uint16_t height) {
    width_ = width;
    height_ = height;
  }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  void write_command(uint8_t command);
  void write_data(uint8_t data);
  void write_array(const uint8_t *data, size_t len);
  void init_display();

  GPIOPin *dc_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *wr_pin_{nullptr};
  GPIOPin *rd_pin_{nullptr};
  GPIOPin *data_pins_[8]{nullptr};
  uint16_t width_{240};
  uint16_t height_{320};
};

}  // namespace ili9xxx
}  // namespace esphome
