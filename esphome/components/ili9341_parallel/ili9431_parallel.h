#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/core/gpio.h"

namespace esphome {
namespace ili9341_parallel {

class ILI9341ParallelDisplay : public Component, public display::DisplayBuffer {
public:
    void set_model(const std::string &model) { model_ = model; }
    void set_dc_pin(GPIOPin *pin) { dc_pin_ = pin; }
    void set_reset_pin(GPIOPin *pin) { reset_pin_ = pin; }
    void set_wr_pin(GPIOPin *pin) { wr_pin_ = pin; }
    void set_rd_pin(GPIOPin *pin) { rd_pin_ = pin; }
    void set_data_pin(int index, GPIOPin *pin) { data_pins_[index] = pin; }

    void setup() override;
    void dump_config() override;
    float get_setup_priority() const override { return setup_priority::PROCESSOR; }
    void display();

protected:
    std::string model_;
    GPIOPin *dc_pin_{nullptr};
    GPIOPin *reset_pin_{nullptr};
    GPIOPin *wr_pin_{nullptr};
    GPIOPin *rd_pin_{nullptr};
    GPIOPin *data_pins_[8]{nullptr};
};

}  // namespace ili9341_parallel
}  // namespace esphome
