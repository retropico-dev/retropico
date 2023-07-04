//
// Created by cpasjuste on 14/06/23.
//

#ifndef MICROBOY_UI_H
#define MICROBOY_UI_H

#include <vector>
#include "rectangle.h"
#include "filer.h"
#include "menu.h"

#define UI_FONT_HEIGHT 16

extern mb::Platform *s_platform;

namespace mb {
    class Ui : public Rectangle {
    public:
        enum Color {
            Black = 0x0000,
            White = 0xFFFF,
            Red = 0xC083,
            Yellow = 0xF5C1,
            Green = 0x0388,
            Blue = 0x01CF,
            Gray = 0x52CB,
            GrayDark = 0x2986,
            Transparent = 0xABCD // fake
        };

        explicit Ui(Platform *platform);

        bool loop();

        static Platform *getPlatform();

        static Display *getDisplay();

    private:
        Platform *p_platform;
        Filer *p_filer;
        Menu *p_menu;
    };
}

#endif //MICROBOY_UI_H
