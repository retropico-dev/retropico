//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_AUDIO_H
#define MICROBOY_AUDIO_H

#include <cstdint>

namespace mb {
    class Audio {
    public:
        explicit Audio() = default;

        virtual ~Audio() = default;

        virtual void setup(uint16_t rate, uint16_t samples) {
            m_rate = rate;
            m_samples = samples;
            printf("Audio::setup: rate: %i, samples: %i\r\n", m_rate, m_samples);
        }

        virtual void play(const void *data, int samples) {};

    protected:
        uint16_t m_rate = 0;
        uint16_t m_samples = 0;
        uint8_t m_volume = 4;
    };
}

#endif //MICROBOY_AUDIO_H
