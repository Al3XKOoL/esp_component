#pragma once

#include "esphome/core/helpers.h"
#include <cinttypes>

namespace esphome {
namespace ili9341_parallel {

static const char *const TAG = "ili9341_parallel";

// Comandos específicos del ILI9341
#define ILI9341_SOFTRESET          0x01
#define ILI9341_SLEEPIN            0x10
#define ILI9341_SLEEPOUT           0x11
#define ILI9341_NORMALDISP         0x13
#define ILI9341_INVERTOFF          0x20
#define ILI9341_INVERTON           0x21
#define ILI9341_GAMMASET           0x26
#define ILI9341_DISPLAYOFF         0x28
#define ILI9341_DISPLAYON          0x29
#define ILI9341_COLADDRSET         0x2A
#define ILI9341_PAGEADDRSET        0x2B
#define ILI9341_MEMORYWRITE        0x2C
#define ILI9341_PIXELFORMAT        0x3A
#define ILI9341_FRAMECONTROL       0xB1
#define ILI9341_DISPLAYFUNC        0xB6
#define ILI9341_ENTRYMODE          0xB7
#define ILI9341_POWERCONTROL1      0xC0
#define ILI9341_POWERCONTROL2      0xC1
#define ILI9341_VCOMCONTROL1       0xC5
#define ILI9341_VCOMCONTROL2       0xC7
#define ILI9341_MEMCONTROL         0x36
#define ILI9341_MADCTL             0x36

#define ILI9341_MADCTL_MY  0x80
#define ILI9341_MADCTL_MX  0x40
#define ILI9341_MADCTL_MV  0x20
#define ILI9341_MADCTL_ML  0x10
#define ILI9341_MADCTL_RGB 0x00
#define ILI9341_MADCTL_BGR 0x08
#define ILI9341_MADCTL_MH  0x04

// Secuencia de inicialización para el ILI9341 en modo paralelo de 8 bits
static const uint8_t PROGMEM INITCMD_ILI9341[] = {
  ILI9341_SOFTRESET, 0,
  ILI9341_POWERCONTROL1, 1, 0x23,
  ILI9341_POWERCONTROL2, 1, 0x10,
  ILI9341_VCOMCONTROL1, 2, 0x2B, 0x2B,
  ILI9341_VCOMCONTROL2, 1, 0xC0,
  ILI9341_MEMCONTROL, 1, ILI9341_MADCTL_MX | ILI9341_MADCTL_BGR,
  ILI9341_PIXELFORMAT, 1, 0x55,
  ILI9341_FRAMECONTROL, 2, 0x00, 0x1B,
  ILI9341_ENTRYMODE, 1, 0x07,
  ILI9341_SLEEPOUT, 0,
  ILI9341_DISPLAYON, 0,
  0x00  // Fin de la lista
};

}  // namespace ili9341_parallel
}  // namespace esphome
