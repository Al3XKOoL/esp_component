#pragma once

// ILI9341 LCD Registers
#define ILI9XXX_NOP         0x00
#define ILI9XXX_SWRESET     0x01
#define ILI9XXX_RDDID       0x04
#define ILI9XXX_RDDST       0x09
#define ILI9XXX_SLPIN       0x10
#define ILI9XXX_SLPOUT      0x11
#define ILI9XXX_PTLON       0x12
#define ILI9XXX_NORON       0x13
#define ILI9XXX_RDMODE      0x0A
#define ILI9XXX_RDMADCTL    0x0B
#define ILI9XXX_RDPIXFMT    0x0C
#define ILI9XXX_RDIMGFMT    0x0D
#define ILI9XXX_RDSELFDIAG  0x0F
#define ILI9XXX_INVOFF      0x20
#define ILI9XXX_INVON       0x21
#define ILI9XXX_GAMMASET    0x26
#define ILI9XXX_DISPOFF     0x28
#define ILI9XXX_DISPON      0x29
#define ILI9XXX_CASET       0x2A
#define ILI9XXX_PASET       0x2B
#define ILI9XXX_RAMWR       0x2C
#define ILI9XXX_RAMRD       0x2E
#define ILI9XXX_PTLAR       0x30
#define ILI9XXX_MADCTL      0x36
#define ILI9XXX_VSCRSADD    0x37
#define ILI9XXX_PIXFMT      0x3A
#define ILI9XXX_FRMCTR1     0xB1
#define ILI9XXX_FRMCTR2     0xB2
#define ILI9XXX_FRMCTR3     0xB3
#define ILI9XXX_INVCTR      0xB4
#define ILI9XXX_DFUNCTR     0xB6
#define ILI9XXX_PWCTR1      0xC0
#define ILI9XXX_PWCTR2      0xC1
#define ILI9XXX_PWCTR3      0xC2
#define ILI9XXX_PWCTR4      0xC3
#define ILI9XXX_PWCTR5      0xC4
#define ILI9XXX_VMCTR1      0xC5
#define ILI9XXX_VMCTR2      0xC7
#define ILI9XXX_RDID1       0xDA
#define ILI9XXX_RDID2       0xDB
#define ILI9XXX_RDID3       0xDC
#define ILI9XXX_RDID4       0xDD
#define ILI9XXX_GMCTRP1     0xE0
#define ILI9XXX_GMCTRN1     0xE1
#define ILI9XXX_COLMOD      0x3A

#include "esphome/core/hal.h"
#include "ili9xxx_defines.h"
#include <pgmspace.h>

namespace esphome {
namespace ili9xxx {

// Secuencia de inicialización para ILI9341 en modo paralelo MCU 8080-8 bits
const uint8_t ILI9341_INIT_CMD[] PROGMEM = {
    ILI9XXX_SWRESET, 0x80,  // Soft reset, esperar 120ms
    0xEF, 3, 0x03, 0x80, 0x02,
    0xCF, 3, 0x00, 0xC1, 0x30,
    0xED, 4, 0x64, 0x03, 0x12, 0x81,
    0xE8, 3, 0x85, 0x00, 0x78,
    0xCB, 5, 0x39, 0x2C, 0x00, 0x34, 0x02,
    0xF7, 1, 0x20,
    0xEA, 2, 0x00, 0x00,
    ILI9XXX_PWCTR1, 1, 0x23,
    ILI9XXX_PWCTR2, 1, 0x10,
    ILI9XXX_VMCTR1, 2, 0x3e, 0x28,
    ILI9XXX_VMCTR2, 1, 0x86,
    ILI9XXX_MADCTL, 1, 0x48,
    ILI9XXX_VSCRSADD, 1, 0x00,
    ILI9XXX_PIXFMT, 1, 0x55,
    ILI9XXX_FRMCTR1, 2, 0x00, 0x18,
    ILI9XXX_DFUNCTR, 3, 0x08, 0x82, 0x27,
    0xF2, 1, 0x00,  // 3Gamma Function Disable
    ILI9XXX_GAMMASET, 1, 0x01,
    ILI9XXX_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09, 0x00,
    ILI9XXX_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36, 0x0F,
    ILI9XXX_SLPOUT, 0x80,  // Salir del modo sleep, esperar 120ms
    ILI9XXX_DISPON, 0x80,  // Encender pantalla, esperar 120ms
    0x00  // Fin de la secuencia
};

}  // namespace ili9xxx
}  // namespace esphome
