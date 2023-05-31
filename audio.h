//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_AUDIO_H
#define MICROBOY_AUDIO_H

extern "C" {
#include "minigb_apu.h"
}

namespace mb {
    class Audio {
    public:
        explicit Audio() = default;

        virtual ~Audio() = default;

        virtual void loop() {};
    };
}

#endif //MICROBOY_AUDIO_H
