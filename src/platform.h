//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PLATFORM_H
#define MICROBOY_PLATFORM_H

#include <cstdio>
#include "display.h"
#include "input.h"
#include "audio.h"
#include "io.h"

namespace mb {
    class Platform {
    public:
        Platform() = default;

        virtual ~Platform() {
            printf("~Platform()\n");
            delete (p_display);
            delete (p_audio);
            delete (p_input);
            delete (p_io);
        };

        Display *getDisplay() { return p_display; };

        Audio *getAudio() { return p_audio; };

        Input *getInput() { return p_input; };

        Io *getIo() { return p_io; };

    protected:
        Display *p_display = nullptr;
        Input *p_input = nullptr;
        Audio *p_audio = nullptr;
        Io *p_io = nullptr;
    };
}

#endif //MICROBOY_PLATFORM_H
