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

        void play(const void *data, int samples) override;

    private:
        //uint16_t m_stream[1098]{};
        i2s_config_t m_i2s_config{};
    };
}

#endif //MICROBOY_AUDIO_PICO_H
