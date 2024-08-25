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
  void fill(Color color); 
  void draw_absolute_pixel_internal(int x, int y, Color color); 
  int get_width_internal(); 
  int get_height_internal(); 

  // Implementación del método de tipo de display
  // Si no se requiere, puedes dejar este método sin implementación
  // o eliminar el override si no tienes un valor definido
  // esphome::display::DisplayType get_display_type() override;

 protected:
  void update(); 

 private:
  TFT_eSPI *tft_;
  uint8_t *buffer_;
  bool auto_clear_ = true;
};

}  // namespace tft_espi
}  // namespace esphome
