//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>

#include "platform.h"
#include "st7789.h"

using namespace mb;

// lcd configuration for https://www.waveshare.com/wiki/Pico-LCD-1.3
const struct st7789_config lcd_config = {
        .spi      = PICO_DEFAULT_SPI_INSTANCE,
        .gpio_din = 11,
        .gpio_clk = 10,
        .gpio_cs  = 9,
        .gpio_dc  = 8,
        .gpio_rst = 12,
        .gpio_bl  = 13,
};

PicoDisplay::PicoDisplay() : Display() {
    printf("PicoDisplay(%ix%i)\r\n", m_size.x, m_size.y);
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

    st7789_set_cursor(x, y);
    st7789_put(color);
}

void PicoDisplay::clear() {
    st7789_fill(0x0000);
}

void PicoDisplay::flip() {
}
