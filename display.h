//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_H
#define MICROBOY_DISPLAY_H

#include "utility.h"

namespace mb {
    class Display {
    public:
        explicit Display(const Utility::Vec2i &size = {160, 144});

        virtual void clear() {};

        virtual void flip() {};

        virtual void drawPixel(const Utility::Vec2i &pos, uint16_t pixel) {};

        virtual void drawLine(uint8_t y, uint16_t *line) {};

    protected:
        Utility::Vec2i m_size{};
    };
}

#endif //MICROBOY_DISPLAY_H
