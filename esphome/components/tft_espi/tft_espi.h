#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "TFT_eSPI.h"

namespace esphome {
namespace tft_espi {

class TFTeSPIDisplay : public display::DisplayBuffer {
 public:
  TFTeSPIDisplay();
  ~TFTeSPIDisplay();

  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void fill(Color color); // No necesita override
  void draw_absolute_pixel_internal(int x, int y, Color color); // No necesita override
  int get_width_internal(); // No necesita override
  int get_height_internal(); // No necesita override

 protected:
  void update(); // Implementa este método si es necesario

 private:
  TFT_eSPI *tft_;
  uint8_t *buffer_;
  bool auto_clear_ = true;
};

}  // namespace tft_espi
}  // namespace esphome
