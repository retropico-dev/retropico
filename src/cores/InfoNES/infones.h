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

        bool loadRom(const std::string &path) override;

        bool loadRom(const uint8_t *buffer, size_t size) override;

        bool loop() override;
    };
}

#endif //MICROBOY_PEANUT_GB_H
