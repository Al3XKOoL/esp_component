#pragma once

#include "esphome/core/hal.h"
#include "ili9xxx_defines.h"
#include <pgmspace.h>

namespace esphome {
namespace ili9xxx {

// Secuencia de inicialización para ILI9341 en modo paralelo MCU 8080-8 bits
const uint8_t ILI9341_INIT_CMD[] PROGMEM = {
    ILI9XXX_SWRESET, 0x80,
    ILI9XXX_DISPOFF, 0x80,
    ILI9XXX_PIXFMT, 1, 0x55,
    ILI9XXX_DISPON, 0x80,
    0x00  // Fin de la lista
};

}  // namespace ili9xxx
}  // namespace esphome
