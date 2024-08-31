#include "ili9341.h"
#include "lcd_com.h"

void ILI9341Component::setup() {
    // Configurar pines de datos
    _d0->setup();
    _d1->setup();
    _d2->setup();
    _d3->setup();
    _d4->setup();
    _d5->setup();
    _d6->setup();
    _d7->setup();

    // Inicialización del LCD
    lcd_interface_cfg(this, _interface);
    lcd_write_register_word(this, 0x01, 0); // Soft reset
    lcd_delay_ms(150);
    lcd_write_register_word(this, 0x28, 0); // Display off
    lcd_write_register_word(this, 0x3A, 0x55); // Pixel format
    lcd_write_register_word(this, 0x29, 0); // Display on
}

void ILI9341Component::loop() {
    // Código de bucle si es necesario
}

void ILI9341Component::set_width(int width) {
    width_ = width;
}

void ILI9341Component::set_height(int height) {
    height_ = height;
}

void ILI9341Component::set_interface(int interface) {
    _interface = interface; // Cambiar 'interface' a '_interface'
}
