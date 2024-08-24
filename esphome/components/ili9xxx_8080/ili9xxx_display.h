#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace ili9xxx {

class ILI9341ParallelDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void dump_config() override;
  
  void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
  void set_wr_pin(GPIOPin *pin) { wr_pin_ = pin; }
  void set_rd_pin(GPIOPin *pin) { rd_pin_ = pin; }
  void set_data_pin(uint8_t index, GPIOPin *pin) { data_pins_[index] = pin; }

 protected:
  void write_command(uint8_t command);
  void write_data(uint8_t data);
  void write_array(const uint8_t *data, size_t len);

  GPIOPin *dc_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *wr_pin_{nullptr};
  GPIOPin *rd_pin_{nullptr};
  GPIOPin *data_pins_[8]{nullptr};
};

}  // namespace ili9xxx
}  // namespace esphome
