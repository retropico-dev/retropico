//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_LINUX_H
#define MICROBOY_DISPLAY_LINUX_H

#include <SDL2/SDL.h>
#include "display.h"
#include "clock.h"

namespace mb {
    class LinuxDisplay : public Display {
    public:
        LinuxDisplay();

        ~LinuxDisplay() override;

        void clear() override;

        void flip() override;

    private:
        SDL_Window *p_window = nullptr;
        SDL_Renderer *p_renderer = nullptr;

        void setPixel(const Utility::Vec2i &pos, uint16_t pixel) override;
    };
}

#endif //MICROBOY_DISPLAY_LINUX_H
