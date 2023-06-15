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

        virtual void setup(uint16_t rate, uint16_t samples, uint8_t channels = 2) {
            m_rate = rate;
            m_samples = samples;
            m_channels = channels;
        }

        virtual void play(const void *data, int samples) {};

    protected:
        uint16_t m_rate = 44100;
        uint16_t m_samples = 735;
        uint8_t m_channels = 2;
        uint8_t m_volume = 1;
    };
}

#endif //MICROBOY_AUDIO_H
