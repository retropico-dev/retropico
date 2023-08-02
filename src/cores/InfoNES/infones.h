//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

namespace mb {
    class InfoNES : public Core {
    public:
        explicit InfoNES(Platform *platform);

        ~InfoNES() override;

        bool loadRom(Io::FileBuffer file) override;

        bool loop(uint16_t buttons) override;
    };
}

#endif //MICROBOY_PEANUT_GB_H
