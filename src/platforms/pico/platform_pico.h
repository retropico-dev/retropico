//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PLATFORM_PICO_H
#define MICROBOY_PLATFORM_PICO_H

#include <pico/stdio.h>
#include <pico/bootrom.h>
#include <pico/multicore.h>
#include <hardware/sync.h>

#include "platform.h"

namespace mb {
    class PicoPlatform : public Platform {
    public:
        PicoPlatform();

        ~PicoPlatform() override;
    };
}

#endif //MICROBOY_PLATFORM_PICO_H
