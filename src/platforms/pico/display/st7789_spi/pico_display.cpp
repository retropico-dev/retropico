//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>

#include "platform.h"
#include "st7789.h"
#include "pico_display.h"

using namespace mb;

// lcd configuration for https://www.waveshare.com/wiki/Pico-LCD-1.3
static const struct st7789_config lcd_config = {
        .spi      = spi0,
        .gpio_din = PIN_DIN,
        .gpio_clk = PIN_CLK,
        .gpio_cs  = PIN_CS,
        .gpio_dc  = PIN_DC,
        .gpio_rst = PIN_RESET,
        .gpio_bl  = PIN_BL,
};

PicoDisplay::PicoDisplay() : Display() {
    printf("PicoDisplay(): %ix%i\r\n", m_size.x, m_size.y);

    // init st7789 display
    st7789_init(&lcd_config, m_size.x, m_size.y);

    // clear the display
    PicoDisplay::clear();
}

void PicoDisplay::setCursor(uint16_t x, uint16_t y) {
    st7789_set_cursor(x, y);
}

void PicoDisplay::setPixel(uint16_t color) {
    st7789_put(color);
}
