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
#include "clock.h"
#include "utility"

namespace mb {
    class Platform {
    public:
        enum RebootTarget {
            Auto,
            Ui,
            Nes,
            Gb
        };

        explicit Platform(bool useDoubleBufferDisplay = false) {}

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

        virtual void reboot(const Platform::RebootTarget &target) {};

    protected:
        Display *p_display = nullptr;
        Input *p_input = nullptr;
        Audio *p_audio = nullptr;
        Io *p_io = nullptr;
    };
}

#ifdef LINUX
#include "platform_linux.h"
#define MBPlatform LinuxPlatform
#else
#include "platform_pico.h"
#define MBPlatform PicoPlatform
#endif

#endif //MICROBOY_PLATFORM_H
