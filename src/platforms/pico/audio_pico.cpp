//
// Created by cpasjuste on 31/05/23.
//

#include "platform.h"

using namespace mb;

PicoAudio::PicoAudio() : Audio() {
}

void PicoAudio::setup(uint16_t rate, uint16_t samples, Audio::AudioCallback cb) {
    Audio::setup(rate, samples, cb);

    // initialize I2S sound driver
    m_i2s_config = i2s_get_default_config();
    m_i2s_config.sample_freq = rate;
    m_i2s_config.dma_trans_count = samples;
    i2s_volume(&m_i2s_config, 2);
    i2s_init(&m_i2s_config);
}

void PicoAudio::play(const void *data, int samples) {
    i2s_dma_write(&m_i2s_config, reinterpret_cast<const int16_t *>(data));
}
