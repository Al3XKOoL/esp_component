// esphome/components/ili9341_parallel/ili9341_parallel.h

#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace ili9341_parallel {

class ILI9341ParallelDisplay : public display::DisplayBuffer, public Component {
 public:
  void set_cs_pin(GPIOPin *pin) { cs_pin_ = pin; }
  void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
  void set_d0_pin(GPIOPin *pin) { d0_pin_ = pin; }
  void set_d1_pin(GPIOPin *pin) { d1_pin_ = pin; }
  void set_d2_pin(GPIOPin *pin) { d2_pin_ = pin; }
  void set_d3_pin(GPIOPin *pin) { d3_pin_ = pin; }
  void set_d4_pin(GPIOPin *pin) { d4_pin_ = pin; }
  void set_d5_pin(GPIOPin *pin) { d5_pin_ = pin; }
  void set_d6_pin(GPIOPin *pin) { d6_pin_ = pin; }
  void set_d7_pin(GPIOPin *pin) { d7_pin_ = pin; }

  void setup() override;
  void loop() override;
  void dump_config() override;

 protected:
  GPIOPin *cs_pin_{nullptr};
  GPIOPin *dc_pin_{nullptr};
  GPIOPin *reset_pin_{nullptr};
  GPIOPin *d0_pin_{nullptr};
  GPIOPin *d1_pin_{nullptr};
  GPIOPin *d2_pin_{nullptr};
  GPIOPin *d3_pin_{nullptr};
  GPIOPin *d4_pin_{nullptr};
  GPIOPin *d5_pin_{nullptr};
  GPIOPin *d6_pin_{nullptr};
  GPIOPin *d7_pin_{nullptr};
};

}  // namespace ili9341_parallel
}  // namespace esphome
