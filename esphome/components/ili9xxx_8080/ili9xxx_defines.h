#pragma once

namespace esphome {
namespace ili9xxx {

// ILI9341 commands
static const uint8_t ILI9XXX_NOP = 0x00;
static const uint8_t ILI9XXX_SWRESET = 0x01;
static const uint8_t ILI9XXX_RDDID = 0x04;
static const uint8_t ILI9XXX_RDDST = 0x09;
static const uint8_t ILI9XXX_SLPIN = 0x10;
static const uint8_t ILI9XXX_SLPOUT = 0x11;
static const uint8_t ILI9XXX_PTLON = 0x12;
static const uint8_t ILI9XXX_NORON = 0x13;
static const uint8_t ILI9XXX_RDMODE = 0x0A;
static const uint8_t ILI9XXX_RDMADCTL = 0x0B;
static const uint8_t ILI9XXX_RDPIXFMT = 0x0C;
static const uint8_t ILI9XXX_RDIMGFMT = 0x0D;
static const uint8_t ILI9XXX_RDSELFDIAG = 0x0F;
static const uint8_t ILI9XXX_INVOFF = 0x20;
static const uint8_t ILI9XXX_INVON = 0x21;
static const uint8_t ILI9XXX_GAMMASET = 0x26;
static const uint8_t ILI9XXX_DISPOFF = 0x28;
static const uint8_t ILI9XXX_DISPON = 0x29;
static const uint8_t ILI9XXX_CASET = 0x2A;
static const uint8_t ILI9XXX_PASET = 0x2B;
static const uint8_t ILI9XXX_RAMWR = 0x2C;
static const uint8_t ILI9XXX_RAMRD = 0x2E;
static const uint8_t ILI9XXX_PTLAR = 0x30;
static const uint8_t ILI9XXX_MADCTL = 0x36;
static const uint8_t ILI9XXX_VSCRSADD = 0x37;
static const uint8_t ILI9XXX_PIXFMT = 0x3A;

}  // namespace ili9xxx
}  // namespace esphome
