#ifndef   __LCD_COM_H__
#define   __LCD_COM_H__

#include "driver/gpio.h"

#define TFTLCD_DELAY	0xFFFF
#define TFTLCD_DELAY16	0xFFFF
#define TFTLCD_DELAY8	0x7F

typedef enum {INTERFACE_I2S, INTERFACE_GPIO, INTERFACE_REG} INTERFACE;

typedef struct {
    int _d0, _d1, _d2, _d3, _d4, _d5, _d6, _d7;
    int _cs, _rs, _wr, _rd, _reset;
    INTERFACE _interface;
} TFT_t;

void lcd_write_data_byte(TFT_t * dev, uint8_t data);
void lcd_write_data_word(TFT_t * dev, uint16_t data);
void lcd_write_addr(TFT_t * dev, uint16_t addr1, uint16_t addr2);
void lcd_write_colors(TFT_t * dev, uint16_t * colors, uint16_t size);
void lcd_delay_ms(int delay_time);
void lcd_write_register_word(TFT_t * dev, uint16_t addr, uint16_t data);
void lcd_write_register_byte(TFT_t * dev, uint8_t addr, uint16_t data);
esp_err_t lcd_interface_cfg(TFT_t * dev, int interface);

#endif
