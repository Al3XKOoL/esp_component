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
  void display();  // Revisa si override es necesario
  void set_brightness(float brightness);  // Revisa si override es necesario
  void fill(Color color);
  void draw_absolute_pixel_internal(int x, int y, Color color) override;
  int get_width_internal() override;
  int get_height_internal() override;
  Color get_pixel_color(int x, int y);  // Agrega esta línea si es necesario
  esphome::display::DisplayType get_display_type() override;  // Si es necesario

 protected:
  void update();  // Implementa este método si es necesario

 private:
  TFT_eSPI *tft_;
  uint8_t *buffer_;
  bool auto_clear_ = true;
};

}  // namespace tft_espi
}  // namespace esphome
