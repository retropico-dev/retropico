//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PLATFORM_H
#define MICROBOY_PLATFORM_H

#include <cstdio>
#include "display.h"
#include "input.h"
#include "audio.h"

namespace mb {
    class Platform {
    public:
        Platform() = default;

        virtual ~Platform() {
            printf("~Platform()\n");
            delete (p_display);
            delete (p_audio);
            delete (p_input);
        };

        Display *getDisplay() { return p_display; };

        Audio *getAudio() { return p_audio; };

        Input *getInput() { return p_input; };

    protected:
        Display *p_display = nullptr;
        Input *p_input = nullptr;
        Audio *p_audio = nullptr;
    };
}

#endif //MICROBOY_PLATFORM_H
