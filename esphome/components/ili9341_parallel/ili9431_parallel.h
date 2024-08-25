#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace ili9341_parallel {

class ILI9341ParallelDisplay : public display::DisplayBuffer, public Component {
 public:
  void set_cs_pin(GPIOPin *pin) { cs_pin_ = pin; }
  void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
  void set_wr_pin(GPIOPin *pin) { wr_pin_ = pin; }
  void set_rd_pin(GPIOPin *pin) { rd_pin_ = pin; }
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

  void setup() override;
  void loop() override;
  void dump_config() override;

  void command(uint8_t value);
  void data(uint8_t value);
  void send_command(uint8_t command_byte, const uint8_t *data_bytes, uint8_t num_data_bytes);
  void update() override;

 protected:
  void setup_pins_();
  void write_byte(uint8_t data);
  void write_array(const uint8_t *data, size_t length);
  void enable();
  void disable();

  GPIOPin *cs_pin_{nullptr};
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *wr_pin_{nullptr};
  GPIOPin *rd_pin_{nullptr};
  GPIOPin *data_pins_[8];
};

}  // namespace ili9341_parallel
}  // namespace esphome
