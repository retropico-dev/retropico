//
// Created by cpasjuste on 08/06/23.
//

#ifndef MICROBOY_ST7789_H
#define MICROBOY_ST7789_H

#define PIN_DIN 19      // SPI0 TX  (MOSI)
#define PIN_CLK 18      // SPI0 SCK
#define PIN_CS 17       // SPI0 CSn
#define PIN_DC 16       // SPI0 RX
#define PIN_RESET 21    // GPIO
#define PIN_BL 20       // GPIO

#define DISPLAY_WIDTH 240
#define DISPLAY_HEIGHT 240

#define SERIAL_CLK_DIV 2.f

#define ST7789_MADCTL_MY 0x80
#define ST7789_MADCTL_MX 0x40
#define ST7789_MADCTL_MV 0x20
#define ST7789_MADCTL_ML 0x10
#define ST7789_MADCTL_BGR 0x08
#define ST7789_MADCTL_MH 0x04
#define ST7789_MADCTL_RGB 0x00

#define ST7789_NOP     0x00
#define ST7789_SWRESET 0x01
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09
#define ST7789_RDDPM   0x0A

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36

#define ST7789_COLOR_MODE_16bit 0x55    //  RGB565 (16bit)
#define ST7789_COLOR_MODE_18bit 0x66    //  RGB666 (18bit)

#define ST7789_ROTATION 2

#if ST7789_ROTATION == 0
#define X_SHIFT 0
#define Y_SHIFT 80
#elif ST7789_ROTATION == 1
#define X_SHIFT 80
#define Y_SHIFT 0
#elif ST7789_ROTATION == 2
#define X_SHIFT 0
#define Y_SHIFT 0
#elif ST7789_ROTATION == 3
#define X_SHIFT 0
#define Y_SHIFT 0
#endif

void st7789_init();

void st7789_start_pixels();

void st7789_set_cursor(uint16_t x, uint16_t y);

void st7789_put(uint16_t pixel);

#endif //MICROBOY_ST7789_H
