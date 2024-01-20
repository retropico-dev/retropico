//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

namespace mb {
    class InfoNES : public Core {
    public:
        explicit InfoNES(p2d::Platform *platform);

        ~InfoNES() override;

        bool loadRom(const p2d::File &file) override;

        bool loop(uint16_t buttons) override;
    };
}

#endif //MICROBOY_PEANUT_GB_H
