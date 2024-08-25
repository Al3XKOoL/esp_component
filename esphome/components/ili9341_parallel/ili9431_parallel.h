#include "esphome.h"
#include "TFT_eSPI.h"

class ILI9341ParallelDisplay : public Component, public DisplayBuffer {
 public:
  TFT_eSPI tft = TFT_eSPI();  // Configura TFT_eSPI en modo paralelo

  void setup() override {
    tft.init();
    tft.setRotation(0);  // Ajusta la rotación
  }

  void update() override {
    tft.fillScreen(TFT_BLACK);
    tft.drawString("Hello, ESPHome!", 10, 10, 2);
  }

  void draw_absolute_pixel_internal(int x, int y, Color color) override {
    uint16_t c = color.to_rgb_565();
    tft.drawPixel(x, y, c);
  }

  int get_width_internal() override { return 240; }
  int get_height_internal() override { return 320; }
};
