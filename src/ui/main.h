//
// Created by cpasjuste on 06/02/24.
//

#ifndef MICROBOY_MAIN_H
#define MICROBOY_MAIN_H

#include <vector>
#include "rectangle.h"
#include "filer.h"
#include "menu.h"
#include "settings.h"
#include "infobox.h"

#define UI_FONT_HEIGHT 8

namespace mb {
    class Ui : public p2d::Rectangle {
    public:
        enum Color : uint16_t {
            Black = 0x0000,
            White = 0xFFFF,
            Red = 0xC083,
            Yellow = 0xFF00,
            YellowLight = 0xF5C1,
            Green = 0x07E0,
            GreenDark = 0x0388,
            Blue = 0x01CF,
            Gray = 0x52CB,
            GrayDark = 0x2986
        };

        explicit Ui(p2d::Platform *platform);

        void onUpdate(p2d::Time delta) override;

        bool onInput(const uint16_t &buttons) override;

        Filer *getFiler() { return p_filer; }

        Menu *getMenu() { return p_menu; }

        Settings *getSettings() { return p_settings; }

        InfoBox *getInfoBox() { return p_infoBox; }

        p2d::Platform *getPlatform();

        static Ui *getInstance();

    private:
        p2d::Platform *p_platform;
        Filer *p_filer;
        Menu *p_menu;
        Settings *p_settings;
        InfoBox *p_infoBox;
        // auto-repeat
        uint16_t m_buttons_old = 0;
        p2d::Clock m_repeat_clock{};
    };
}

#endif //MICROBOY_MAIN_H
