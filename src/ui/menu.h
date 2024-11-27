//
// Created by cpasjuste on 04/07/23.
//

#ifndef RETROPICO_MENU_H
#define RETROPICO_MENU_H

#include "rectangle.h"
#include "text.h"
#include "bitmap.h"

namespace retropico {
    class Menu : public p2d::Rectangle {
    public:
        class MenuLine : public Rectangle {
        public:
            MenuLine(const p2d::Utility::Vec4i &bounds, const p2d::Io::File &file, const std::string &text);

            p2d::Text *p_text;
            p2d::Bitmap *p_bitmap;
        };

        Menu(const p2d::Utility::Vec2i &pos, const p2d::Utility::Vec2i &size);

        void setSelection(uint8_t index);

    private:
        std::vector<MenuLine *> m_lines;
        int8_t m_line_index = 0;

        void refresh() const;

        bool onInput(const uint16_t &buttons) override;
    };
}

#endif //RETROPICO_MENU_H
