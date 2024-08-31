#include "ili9341.h"
#include "lcd_com.h"

void ILI9341Component::setup() {
    // Inicialización del LCD
    lcd_interface_cfg(this, interface_);
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
    interface_ = interface;
}
