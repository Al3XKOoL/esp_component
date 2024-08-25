#pragma once

#include "esphome/core/component.h"
#include "TFT_eSPI.h"

// Asegúrate de ajustar el tamaño del display y otros parámetros aquí
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

class TFT_ESPIComponent : public esphome::Component {
 public:
  TFT_ESPIComponent() : tft_() {}

  void setup() override {
    // Inicializa la pantalla TFT
    tft_.init();
    tft_.setRotation(0);  // Ajusta la rotación según tu pantalla
    tft_.fillScreen(TFT_BLACK);  // Limpia la pantalla con color negro
  }

  void loop() override {
    // Llama a tus actualizaciones de pantalla aquí si es necesario
  }

  void draw_text(int16_t x, int16_t y, const String &text, uint16_t color) {
    tft_.setTextColor(color);
    tft_.setCursor(x, y);
    tft_.print(text);
  }

 private:
  TFT_eSPI tft_;
};
