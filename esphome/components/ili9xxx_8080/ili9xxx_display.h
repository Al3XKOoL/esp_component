// ili9xxx_display.h
#pragma once

#include "esphome/components/display/display_buffer.h"
#include "esphome/components/display/display_color_utils.h"
#include "ili9xxx_init.h"
#include "ili9xxx_defines.h"

namespace esphome {
namespace ili9xxx {

class ILI9341Display : public display::DisplayBuffer, public Component {
 public:
   void setup() override;
   void dump_config() override;

 protected:
   void write_command(uint8_t command);
   void write_data(uint8_t data);
   void write_array(const uint8_t *data, size_t len);

   GPIOPin *dc_pin_{nullptr};
   GPIOPin *reset_pin_{nullptr};
   GPIOPin *wr_pin_{nullptr};
   GPIOPin *rd_pin_{nullptr};
   std::array<GPIOPin*, 8> data_pins_{};
};

}  // namespace ili9xxx
}  // namespace esphome
