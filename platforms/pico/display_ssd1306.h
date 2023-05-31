//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_SSD1306_H
#define MICROBOY_DISPLAY_SSD1306_H

#include "display.h"
#include "ssd1306.h"
#include "clock.h"

namespace mb {
    class SSD1306Display : public Display {
    public:
        SSD1306Display();

        void clear() override;

        void flip() override;

        void drawLine(uint8_t y, uint16_t *line) override;

    private:
        ssd1306_t m_display{};
    };
}

#endif //MICROBOY_DISPLAY_SSD1306_H
