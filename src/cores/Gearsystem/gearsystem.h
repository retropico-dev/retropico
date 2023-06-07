//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

namespace mb {
    class GearSystem : public Core {
    public:
        explicit GearSystem(Platform *platform);

        ~GearSystem() override;

        bool loadRom(const std::string &path) override;

        bool loadRom(const uint8_t *buffer, size_t size) override;

        bool loop() override;

    private:
        Surface *p_surface = nullptr;
        Utility::Vec2i m_dstSize;
        Utility::Vec2i m_dstPos;
    };
}

#endif //MICROBOY_PEANUT_GB_H
