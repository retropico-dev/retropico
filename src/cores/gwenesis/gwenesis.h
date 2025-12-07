//
// Created by cpasjuste on 06/06/23.
//

#ifndef RETROPICO_SMS_H
#define RETROPICO_SMS_H

#include "core.h"

namespace retropico {
    class Gwenesis final : public Core {
    public:
        explicit Gwenesis(const p2d::Display::Settings &displaySettings);

        bool loop() override;

        void close() override;

        bool loadRom(const p2d::Io::File &file) override;
    };
}

#endif //RETROPICO_SMS_H
