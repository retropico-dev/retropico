//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_SAMEBOY_H
#define MICROBOY_SAMEBOY_H

#include "core.h"

namespace mb {
    class SameBoy : public Core {
    public:
        explicit SameBoy(Platform *platform);

        bool loadRom(const uint8_t *buffer, size_t size) override;

        void loop() override;
    };
}

#endif //MICROBOY_SAMEBOY_H
