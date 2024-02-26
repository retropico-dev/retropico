//
// Created by cpasjuste on 06/06/23.
//

#ifndef RETROPICO_INFONES_H
#define RETROPICO_INFONES_H

#include "core.h"

namespace mb {
    class InfoNES : public Core {
    public:
        explicit InfoNES(const p2d::Display::Settings &displaySettings);

        bool loop() override;

        void close() override;

        bool loadRom(const p2d::Io::File &file) override;
    };
}

#endif //RETROPICO_INFONES_H
