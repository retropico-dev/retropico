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

        virtual void drawPixel(const Utility::Vec2i &pos, uint16_t pixel) {};

        Utility::Vec2i getSize() { return m_size; };

    protected:
        Utility::Vec2i m_size{};
    };
}

#endif //MICROBOY_DISPLAY_H
