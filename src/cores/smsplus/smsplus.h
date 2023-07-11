//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

namespace mb {
    class SMSPlus : public Core {
    public:
        explicit SMSPlus(Platform *platform);

        bool loadRom(const std::string &path) override;

        bool loadRom(Io::FileBuffer file) override;

        bool loop() override;
    };
}

#endif //MICROBOY_PEANUT_GB_H
