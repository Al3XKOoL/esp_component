#include "lcd_com.h"
#include "driver/gpio.h"

void lcd_write_comm_byte(TFT_t * dev, uint8_t cmd) {
    gpio_set_level(dev->_cs, 0);
    gpio_set_level(dev->_rs, 0);
    // Enviar comando
    gpio_set_level(dev->_d0, (cmd >> 0) & 0x01);
    gpio_set_level(dev->_d1, (cmd >> 1) & 0x01);
    gpio_set_level(dev->_d2, (cmd >> 2) & 0x01);
    gpio_set_level(dev->_d3, (cmd >> 3) & 0x01);
    gpio_set_level(dev->_d4, (cmd >> 4) & 0x01);
    gpio_set_level(dev->_d5, (cmd >> 5) & 0x01);
    gpio_set_level(dev->_d6, (cmd >> 6) & 0x01);
    gpio_set_level(dev->_d7, (cmd >> 7) & 0x01);
    gpio_set_level(dev->_wr, 0); // Escribir
    gpio_set_level(dev->_wr, 1); // Fin de escritura
    gpio_set_level(dev->_cs, 1);
}

void lcd_write_data_byte(TFT_t * dev, uint8_t data) {
    gpio_set_level(dev->_cs, 0);
    gpio_set_level(dev->_rs, 1);
    // Enviar datos
    gpio_set_level(dev->_d0, (data >> 0) & 0x01);
    gpio_set_level(dev->_d1, (data >> 1) & 0x01);
    gpio_set_level(dev->_d2, (data >> 2) & 0x01);
    gpio_set_level(dev->_d3, (data >> 3) & 0x01);
    gpio_set_level(dev->_d4, (data >> 4) & 0x01);
    gpio_set_level(dev->_d5, (data >> 5) & 0x01);
    gpio_set_level(dev->_d6, (data >> 6) & 0x01);
    gpio_set_level(dev->_d7, (data >> 7) & 0x01);
    gpio_set_level(dev->_wr, 0); // Escribir
    gpio_set_level(dev->_wr, 1); // Fin de escritura
    gpio_set_level(dev->_cs, 1);
}

void lcd_write_data_word(TFT_t * dev, uint16_t data) {
    lcd_write_data_byte(dev, (data >> 8) & 0xFF); // Enviar byte alto
    lcd_write_data_byte(dev, data & 0xFF);        // Enviar byte bajo
}

// Implementar otras funciones como lcd_write_data_byte, lcd_write_data_word, etc.

void lcdDrawPixel(TFT_t * dev, uint16_t x, uint16_t y, uint16_t color) {
    lcd_write_comm_byte(dev, 0x2A); // Column Address Set
    lcd_write_data_byte(dev, (x >> 8) & 0xFF);
    lcd_write_data_byte(dev, x & 0xFF);
    lcd_write_comm_byte(dev, 0x2B); // Row Address Set
    lcd_write_data_byte(dev, (y >> 8) & 0xFF);
    lcd_write_data_byte(dev, y & 0xFF);
    lcd_write_comm_byte(dev, 0x2C); // Memory Write
    lcd_write_data_byte(dev, (color >> 8) & 0xFF);
    lcd_write_data_byte(dev, color & 0xFF);
}
