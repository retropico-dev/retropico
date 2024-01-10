//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

namespace mb {
    class SMSPlus : public Core {
    public:
        explicit SMSPlus(p2d::Platform *platform);

        bool loadRom(p2d::Io::FileBuffer file) override;

        bool loop(uint16_t buttons) override;
    };
}

#endif //MICROBOY_PEANUT_GB_H
