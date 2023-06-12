//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_AUDIO_PICO_H
#define MICROBOY_AUDIO_PICO_H

#include "i2s.h"

namespace mb {
    class PicoAudio : public Audio {
    public:
        explicit PicoAudio();

        void setup(uint16_t rate, uint16_t samples, AudioCallback cb) override;

        void play(const void *data, int samples = 0) override;

    private:
        i2s_config_t m_i2s_config{};
    };
}

#endif //MICROBOY_AUDIO_PICO_H
