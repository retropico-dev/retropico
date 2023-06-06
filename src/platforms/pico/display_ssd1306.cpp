//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>
#include <hardware/i2c.h>
#include <hardware/gpio.h>

#include "platform.h"

using namespace mb;

SSD1306Display::SSD1306Display() : Display({SSD1306_WIDTH, SSD1306_HEIGHT}) {
    // init SSD1306 oled panel (debug, cropped screen)
    uint rate = i2c_init(i2c0, 25 * 100000); // push i2c rate (25Mhz)
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA_PIN);
    gpio_pull_up(SSD1306_SCL_PIN);

    ssd1306_init(&m_display, SSD1306_WIDTH, SSD1306_HEIGHT, 0x3C, i2c0);
    ssd1306_clear(&m_display);
    ssd1306_show(&m_display);
    printf("SSD1306Display(): i2c baud rate set to %i MHz\r\n", rate / 100000);
}

void SSD1306Display::drawPixel(int16_t x, int16_t y, uint16_t color) {
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

    if (color > 0) ssd1306_draw_pixel(&m_display, x, y);
    else ssd1306_clear_pixel(&m_display, x, y);
}

void SSD1306Display::clear() {
    ssd1306_clear(&m_display);
}

void SSD1306Display::flip() {
    ssd1306_show(&m_display);
}
