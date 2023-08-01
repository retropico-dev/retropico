//
// Created by cpasjuste on 14/06/23.
//

#ifndef MICROBOY_UI_H
#define MICROBOY_UI_H

#include <vector>
#include "rectangle.h"
#include "filer.h"
#include "menu.h"
#include "settings.h"

#define UI_FONT_HEIGHT 16

namespace mb {
    class Ui : public Rectangle {
    public:
        enum Color {
            Black = 0x0000,
            White = 0xFFFF,
            Red = 0xC083,
            Yellow = 0xFF00,
            YellowLight = 0xF5C1,
            Green = 0x07E0,
            GreenDark = 0x0388,
            Blue = 0x01CF,
            Gray = 0x52CB,
            GrayDark = 0x2986,
            Transparent = 0xABCD // fake
        };

        explicit Ui(Platform *platform);

        bool loop(bool force = false);

        Filer *getFiler() { return p_filer; }

        Menu *getMenu() { return p_menu; }

        Settings *getSettings() { return p_settings; };

        static Ui *getInstance();

        static Platform *getPlatform();

        static Display *getDisplay();

    private:
        Platform *p_platform;
        Filer *p_filer;
        Menu *p_menu;
        Settings *p_settings;
        // auto-repeat
        uint16_t m_buttons_old = 0;
        Clock m_repeat_clock{};
    };
}

#endif //MICROBOY_UI_H
