//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PLATFORM_LINUX_H
#define MICROBOY_PLATFORM_LINUX_H

#include <SDL2/SDL.h>
#include "platform.h"
#include "platform_compat.h"

#define _Noreturn
#define tight_loop_contents() ((void)0)
#define stdio_flush() fflush(stdout)
#define reset_usb_boot(x, y) ((void)0)
#define __wfi() break

namespace mb {
    class LinuxPlatform : public Platform {
    public:
        LinuxPlatform();

        ~LinuxPlatform() override;
    };
}

#endif //MICROBOY_PLATFORM_LINUX_H
