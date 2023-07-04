//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PLATFORM_LINUX_H
#define MICROBOY_PLATFORM_LINUX_H

#include <SDL2/SDL.h>
#include "display_linux.h"
#include "input_linux.h"
#include "audio_linux.h"
#include "io_linux.h"
#include "pico.h"

namespace mb {
    class LinuxPlatform : public Platform {
    public:
        LinuxPlatform(bool useDoubleBufferDisplay = false);

        ~LinuxPlatform() override;
    };
}

#endif //MICROBOY_PLATFORM_LINUX_H
