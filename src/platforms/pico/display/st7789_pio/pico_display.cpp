//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>

#include "platform.h"
#include "pico_display.h"
#include "st7789.h"

using namespace mb;

PicoDisplay::PicoDisplay() : Display({DISPLAY_WIDTH, DISPLAY_HEIGHT}) {
    printf("PicoDisplay(): %ix%i\r\n", m_size.x, m_size.y);

    st7789_init();
    // screen rotation
    //rotation = 1;
    PicoDisplay::clear();
}

// very slow, obviously...
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

// very fast :)
void PicoDisplay::drawPixelLine(uint16_t x, uint16_t y, uint16_t width, const uint16_t *pixels) {
    st7789_set_cursor(x, y);
    for (int_fast16_t i = 0; i < width; i++) {
        st7789_put(pixels[i]);
    }
}

void PicoDisplay::clear() {
    st7789_start_pixels();
    for (int y = 0; y < m_size.x; ++y) {
        for (int x = 0; x < m_size.y; ++x) {
            st7789_put(0x0000);
        }
    }
}

void PicoDisplay::flip() {
}
