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

// Implementar otras funciones como lcd_write_data_byte, lcd_write_data_word, etc.
