//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_H
#define MICROBOY_DISPLAY_H

#include "utility.h"

namespace mb {
    class Display {
    public:
        explicit Display(const Utility::Vec2i &size = {160, 144}) {
            m_size = size;
        };

        virtual ~Display() = default;

        virtual void clear() {};

        virtual void flip() {};

        virtual void drawLine(uint8_t y, uint8_t width, uint16_t *line) {};

    protected:
        Utility::Vec2i m_size{};
    };
}

#endif //MICROBOY_DISPLAY_H
