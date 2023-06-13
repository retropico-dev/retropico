//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_AUDIO_PICO_H
#define MICROBOY_AUDIO_PICO_H

#include <pico/audio_i2s.h>

namespace mb {
    class PicoAudio : public Audio {
    public:
        explicit PicoAudio();

        void setup(uint16_t rate, uint16_t samples) override;

        void play(const void *data, int samples) override;

    private:
        audio_i2s_config_t m_i2s_config{};
        audio_buffer_pool *p_producer_pool;
    };
}

#endif //MICROBOY_AUDIO_PICO_H
