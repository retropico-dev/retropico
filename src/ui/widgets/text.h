//
// Created by cpasjuste on 30/03/23.
//

#ifndef U_GAMEPAD_TEXT_H
#define U_GAMEPAD_TEXT_H

#include "widget.h"
#include "rectangle.h"

namespace mb {
    class Text : public Widget {
    public:
        Text(int16_t x, int16_t y, const std::string &text,
             uint16_t color = Display::Color::White);

        Text(const Utility::Vec2i &pos, const std::string &text,
             uint16_t color = Display::Color::White);

        std::string getString();

        void setString(const std::string &str);

        void loop(const Utility::Vec2i &pos, const uint16_t &buttons) override;

    private:
        std::string m_text;
        Utility::Vec4i m_bounds;
    };
}

#endif //U_GAMEPAD_TEXT_H
