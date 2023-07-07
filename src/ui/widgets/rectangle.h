//
// Created by cpasjuste on 30/03/23.
//

#ifndef U_MICROBOY_RECTANGLE_H
#define U_MICROBOY_RECTANGLE_H

#include "widget.h"

namespace mb {
    class Rectangle : public Widget {
    public:
        Rectangle(int16_t x, int16_t y, int16_t w, int16_t h,
                  uint16_t color = Display::Color::White, int16_t radius = 0);

        Rectangle(const Utility::Vec2i &pos, const Utility::Vec2i &size,
                  uint16_t color = Display::Color::White, int16_t radius = 0);

        explicit Rectangle(const Utility::Vec4i &bounds,
                           uint16_t color = Display::Color::White, int16_t radius = 0);

        void loop(const Utility::Vec2i &pos, const uint16_t &buttons) override;

    private:
        int16_t m_radius = 0;
    };
}

#endif //U_MICROBOY_RECTANGLE_H
