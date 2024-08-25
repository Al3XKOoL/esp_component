#pragma once

#include "esphome.h"
#include "TFT_eSPI.h"

namespace esphome {
namespace tft_espi {

class TFTeSPIDisplay : public Component, public display::DisplayBuffer {
 public:
  TFTeSPIDisplay();  // Constructor
  ~TFTeSPIDisplay(); // Destructor
  
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void display() override;
  
  void set_rotation(uint8_t rotation);
  void set_invert(bool invert);
  void fill(Color color);
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  void set_brightness(float brightness) override;
  
 protected:
  Color get_pixel_color(int x, int y);

 private:
  TFT_eSPI *tft_;   // Puntero a TFT_eSPI
  uint8_t *buffer_; // Buffer de datos de pantalla
  bool auto_clear_ = true; // Si se debe limpiar la pantalla automáticamente
};

}  // namespace tft_espi
}  // namespace esphome
