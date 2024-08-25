#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display.h"
#include "TFT_eSPI.h"

class TFT_ESPIComponent : public esphome::display::DisplayBuffer, public esphome::Component {
 public:
  TFT_ESPIComponent() : tft_() {}

  void setup() override {
    tft_.init();
    tft_.setRotation(0);  // Ajusta la rotación según tu pantalla
    tft_.fillScreen(TFT_BLACK);  // Limpia la pantalla con color negro
  }

  void draw_abs(int x, int y, const esphome::display::DisplayBuffer &buffer) override {
    // Implementa cómo dibujar el buffer en tu pantalla TFT
    // Esto puede incluir la lógica para convertir el buffer en imágenes o texto
  }

  void draw_text(int16_t x, int16_t y, const std::string &text, uint16_t color) {
    tft_.setTextColor(color);
    tft_.setCursor(x, y);
    tft_.print(text.c_str());
  }

 private:
  TFT_eSPI tft_;
};
