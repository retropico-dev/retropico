//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>

#include "platform.h"
#include "st7789.h"
#include "pico_display.h"

using namespace mb;

// lcd configuration for https://www.waveshare.com/wiki/Pico-LCD-1.3
const struct st7789_config lcd_config = {
        .spi      = spi0,
        .gpio_din = PIN_DIN,
        .gpio_clk = PIN_CLK,
        .gpio_cs  = PIN_CS,
        .gpio_dc  = PIN_DC,
        .gpio_rst = PIN_RESET,
        .gpio_bl  = PIN_BL,
};

PicoDisplay::PicoDisplay() : Display() {
    printf("PicoDisplay(%ix%i)\r\n", m_size.x, m_size.y);

    //p_pixelBuffer = (uint8_t *) malloc(m_size.x * m_size.y * 2);
    //memset(p_pixelBuffer, 0x00, m_size.x * m_size.y * 2);

    st7789_init(&lcd_config, m_size.x, m_size.y);
    st7789_fill(0x0000);
}

void PicoDisplay::drawPixel(int16_t x, int16_t y, uint16_t color) {
    if ((x < 0) || (y < 0) || x > m_size.x || y > m_size.y) return;

    int16_t t;
    switch (rotation) {
        case 1:
            t = x;
            x = (int16_t) (m_size.x - 1 - y);
            y = t;
            break;
        case 2:
            x = (int16_t) (m_size.x - 1 - x);
            y = (int16_t) (m_size.y - 1 - y);
            break;
        case 3:
            t = x;
            x = y;
            y = (int16_t) (m_size.y - 1 - t);
            break;
    }

    //*(uint16_t *) (p_pixelBuffer + y * m_pitch + x * m_bpp) = color;
    st7789_set_cursor(x, y);
    st7789_put(color);
}

/*
void PicoDisplay::drawSurface(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size) {
    //st7789_set_cursor(0, 0);
    //st7789_write(surface->getPixels(), surface->getPitch() * surface->getSize().y);
    auto pixels = surface->getPixels();
    auto pitch = surface->getPitch();
    auto bpp = surface->getBpp();
    auto width = surface->getSize().x;
    auto height = surface->getSize().y;
    for (int_fast8_t y = 0; y < height; y++) {
        st7789_set_cursor(pos.x, pos.y + y);
        st7789_write(pixels + y * pitch, width * bpp);
    }
}
*/

void PicoDisplay::drawPixelLine(uint16_t x, uint16_t y, uint16_t width, const uint16_t *pixels) {
    st7789_set_cursor(x, y);
    st7789_write(pixels, width * 2);
}

void PicoDisplay::clear() {
    st7789_fill(0x0000);
}

void PicoDisplay::flip() {
    //st7789_set_cursor(0, 0);
    //st7789_write(p_pixelBuffer, m_size.y * m_pitch);
}
