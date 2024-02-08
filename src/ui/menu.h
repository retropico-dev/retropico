//
// Created by cpasjuste on 04/07/23.
//

#ifndef MICROBOY_MENU_H
#define MICROBOY_MENU_H

#include "rectangle.h"
#include "text.h"
#include "bitmap.h"

namespace mb {
    class Menu : public p2d::Rectangle {
    public:
        class MenuLine : public Rectangle {
        public:
            MenuLine(const p2d::Utility::Vec4i &bounds, const p2d::Io::File &file, const std::string &text);

            p2d::Text *p_text;
            p2d::Bitmap *p_bitmap;
        };

        Menu(const p2d::Utility::Vec2i &pos, const p2d::Utility::Vec2i &size);

    private:
        std::vector<MenuLine *> m_lines;
        int8_t m_line_index = 0;

        void refresh();

        bool onInput(const uint16_t &buttons) override;
    };
}

#endif //MICROBOY_MENU_H
