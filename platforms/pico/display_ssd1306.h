//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_SSD1306_H
#define MICROBOY_DISPLAY_SSD1306_H

#include "display.h"
#include "ssd1306.h"
#include "clock.h"

#define SSD1306_SDA_PIN 0
#define SSD1306_SCL_PIN 1

#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64

namespace mb {
    class SSD1306Display : public Display {
    public:
        SSD1306Display();

        void clear() override;

        void flip() override;

    private:
        ssd1306_t m_display{};

        void setPixel(const Utility::Vec2i &pos, uint16_t pixel) override;
    };
}

#endif //MICROBOY_DISPLAY_SSD1306_H
