//
// Created by cpasjuste on 06/02/24.
//

#ifndef RETROPICO_MAIN_H
#define RETROPICO_MAIN_H

#include "retropico_config.h"
#include "rectangle.h"
#include "filer.h"
#include "menu.h"
#include "settings.h"

#define UI_FONT_HEIGHT 8

namespace retropico {
    class Ui : public p2d::Rectangle {
    public:
        explicit Ui(p2d::Platform *platform);

        void onUpdate(p2d::Time delta) override;

        bool onInput(const uint16_t &buttons) override;

        Config *getConfig() { return p_config; }

        Filer *getFiler() { return p_filer; }

        Menu *getMenu() { return p_menu; }

        Settings *getSettings() { return p_settings; }

        InfoBox *getInfoBox() { return p_infoBox; }

        p2d::Platform *getPlatform();

        bool isDone() { return p_filer->isDone(); }

        static Ui *getInstance();

    private:
        p2d::Platform *p_platform;
        Config *p_config;
        Overlay *p_overlay;
        Filer *p_filer;
        Menu *p_menu;
        Settings *p_settings;
        InfoBox *p_infoBox;
        // auto-repeat
        uint16_t m_buttons_old = 0;
        p2d::Clock m_repeat_clock{};
    };
}

#endif //RETROPICO_MAIN_H
