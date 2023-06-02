//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>
#include <hardware/i2c.h>
#include <hardware/gpio.h>
#include "display.h"
#include "display_ssd1306.h"

using namespace mb;

SSD1306Display::SSD1306Display() : Display() {
    // init SSD1306 oled panel (debug, cropped screen)
    uint rate = i2c_init(i2c0, 1300000); // push i2c rate for speed
    gpio_set_function(SSD1306_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(SSD1306_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(SSD1306_SDA_PIN);
    gpio_pull_up(SSD1306_SCL_PIN);

    ssd1306_init(&m_display, 128, 64, 0x3C, i2c0);
    SSD1306Display::clear();
    SSD1306Display::flip();
    printf("SSD1306Display(): i2c baud rate set to %i kHz\r\n", rate / 1000);
}

void SSD1306Display::drawPixel(const Utility::Vec2i &pos, uint16_t pixel) {
    if (pixel > 0) ssd1306_draw_pixel(&m_display, pos.x, pos.y);
    else ssd1306_clear_pixel(&m_display, pos.x, pos.y);
}

void SSD1306Display::clear() {
    ssd1306_clear(&m_display);
}

void SSD1306Display::flip() {
    ssd1306_show(&m_display);
}
