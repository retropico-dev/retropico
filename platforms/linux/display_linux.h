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

        void drawPixel(uint16_t x, uint16_t y, uint16_t color) override;

    private:
        SDL_Window *p_window = nullptr;
        SDL_Renderer *p_renderer = nullptr;
    };
}

#endif //MICROBOY_DISPLAY_LINUX_H
