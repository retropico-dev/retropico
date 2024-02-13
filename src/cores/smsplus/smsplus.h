//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

namespace mb {
    class SMSPlus : public Core {
    public:
        explicit SMSPlus(const p2d::Display::Settings &displaySettings);

        bool loop() override;

        //void close() override;

        bool loadRom(const p2d::Io::File &file) override;
    };
}

#endif //MICROBOY_PEANUT_GB_H
