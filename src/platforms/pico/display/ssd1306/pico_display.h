//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PICO_DISPLAY_H
#define MICROBOY_PICO_DISPLAY_H

#include "ssd1306.h"

#define SSD1306_SDA_PIN 0
#define SSD1306_SCL_PIN 1

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

namespace mb {
    class PicoDisplay : public Display {
    public:
        PicoDisplay();

        void clear() override;

        void flip() override;

        void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    private:
        ssd1306_t m_display{};
    };
}

#endif //MICROBOY_PICO_DISPLAY_H
