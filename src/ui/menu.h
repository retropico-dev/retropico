//
// Created by cpasjuste on 04/07/23.
//

#ifndef MICROBOY_MENU_H
#define MICROBOY_MENU_H

#include "rectangle.h"
#include "text.h"
#include "bitmap.h"

namespace mb {
    class Menu : public Rectangle {
    public:
        class MenuLine : public Rectangle {
        public:
            MenuLine(const Utility::Vec4i &bounds, const Bitmap::Image *image, const std::string &text);

            Text *p_text;
            Bitmap *p_bitmap;
        };

        Menu(const Utility::Vec2i &pos, const Utility::Vec2i &size);

    private:
        std::vector<MenuLine *> m_lines;
        int8_t m_line_index = 0;

        void refresh();

        void loop(const Utility::Vec2i &pos, const uint16_t &buttons) override;
    };
}

#endif //MICROBOY_MENU_H
