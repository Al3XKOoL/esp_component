#pragma once

namespace esphome {
namespace ili9xxx {

// Color definitions
static const uint8_t MADCTL_MY    = 0x80;
static const uint8_t MADCTL_MX    = 0x40;
static const uint8_t MADCTL_MV    = 0x20;
static const uint8_t MADCTL_ML    = 0x10;
static const uint8_t MADCTL_RGB   = 0x00;
static const uint8_t MADCTL_BGR   = 0x08;
static const uint8_t MADCTL_MH    = 0x04;

// Comandos específicos del ILI9341 para modo paralelo MCU 8080-8 bits
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
static const uint8_t ILI9XXX_FRMCTR1 = 0xB1;
static const uint8_t ILI9XXX_FRMCTR2 = 0xB2;
static const uint8_t ILI9XXX_FRMCTR3 = 0xB3;
static const uint8_t ILI9XXX_INVCTR = 0xB4;
static const uint8_t ILI9XXX_DFUNCTR = 0xB6;
static const uint8_t ILI9XXX_PWCTR1 = 0xC0;
static const uint8_t ILI9XXX_PWCTR2 = 0xC1;
static const uint8_t ILI9XXX_PWCTR3 = 0xC2;
static const uint8_t ILI9XXX_PWCTR4 = 0xC3;
static const uint8_t ILI9XXX_PWCTR5 = 0xC4;
static const uint8_t ILI9XXX_VMCTR1 = 0xC5;
static const uint8_t ILI9XXX_VMCTR2 = 0xC7;
static const uint8_t ILI9XXX_RDID1 = 0xDA;
static const uint8_t ILI9XXX_RDID2 = 0xDB;
static const uint8_t ILI9XXX_RDID3 = 0xDC;
static const uint8_t ILI9XXX_RDID4 = 0xDD;
static const uint8_t ILI9XXX_GMCTRP1 = 0xE0;
static const uint8_t ILI9XXX_GMCTRN1 = 0xE1;

// Nuevas definiciones para los comandos faltantes
static const uint8_t ILI9XXX_PWCTRB = 0xCF;
static const uint8_t ILI9XXX_PWRONCTRL = 0xED;
static const uint8_t ILI9XXX_DTCTRLA = 0xE8;
static const uint8_t ILI9XXX_PWCTRSEQ = 0xCB;
static const uint8_t ILI9XXX_PUMPCTRL = 0xF7;
static const uint8_t ILI9XXX_DTCTRLB = 0xEA;

}  // namespace ili9xxx
}  // namespace esphome
