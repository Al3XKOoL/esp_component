#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace ili9341_parallel {

class ILI9341ParallelDisplay : public display::DisplayBuffer, public Component {
 public:
  void set_model(const std::string &model) { model_ = model; }
  void set_width(int16_t width) { width_ = width; }
  void set_height(int16_t height) { height_ = height; }
  void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
  void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
  void set_wr_pin(GPIOPin *pin) { wr_pin_ = pin; }
  void set_rd_pin(GPIOPin *pin) { rd_pin_ = pin; }
  void set_data_pin(int index, GPIOPin *pin) { data_pins_[index] = pin; }

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

 protected:
  std::string model_;
  int16_t width_;
  int16_t height_;
  GPIOPin *dc_pin_;
  GPIOPin *reset_pin_;
  GPIOPin *wr_pin_;
  GPIOPin *rd_pin_;
  GPIOPin *data_pins_[8];
};

}  // namespace ili9341_parallel
}  // namespace esphome
