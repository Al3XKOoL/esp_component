#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include <TFT_eSPI.h>

namespace esphome {
namespace tft_espi {

class TFTeSPIDisplay : public display::DisplayBuffer {
 public:
  void setup() override;
  void display();
  void update() override;
  void fill(Color color) override;
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  void set_brightness(float brightness);
  void set_auto_clear(bool auto_clear) { this->auto_clear_ = auto_clear; }
  
  // Cambia esta línea
  Color get_pixel_color(int x, int y);  // Elimina 'override'

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_COLOR; }

 protected:
    TFT_eSPI *tft_{nullptr};
    uint8_t *buffer_{nullptr};  // Asegúrate de que esto esté declarado
    bool auto_clear_{true};  // Agregar soporte para auto_clear_enabled
};

}  // namespace tft_espi
}  // namespace esphome
