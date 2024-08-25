#include "esphome.h"
#include "TFT_eSPI.h"

namespace esphome {
namespace ili9341_parallel {

class ILI9341ParallelDisplay : public Component, public display::DisplayBuffer {
 public:
  TFT_eSPI tft = TFT_eSPI();

  void setup() override {
    tft.init();
    tft.setRotation(rotation_);
  }

  void draw_absolute_pixel_internal(int x, int y, Color color) override {
    uint16_t c = color.to_rgb_565();
    tft.drawPixel(x, y, c);
  }

  int get_width_internal() override { return 240; }
  int get_height_internal() override { return 320; }

  void set_rotation(uint8_t rotation) { this->rotation_ = rotation; }

 protected:
  uint8_t rotation_;
};

}  // namespace ili9341_parallel
}  // namespace esphome
