//
// Created by cpasjuste on 31/05/23.
//

#include "audio_pico.h"
#include "i2s.h"

using namespace mb;

PicoAudio::PicoAudio() : Audio() {
    // initialize I2S sound driver
    m_i2s_config = i2s_get_default_config();
    m_i2s_config.sample_freq = AUDIO_SAMPLE_RATE;
    m_i2s_config.dma_trans_count = AUDIO_SAMPLES;
    i2s_volume(&m_i2s_config, 7);
    i2s_init(&m_i2s_config);

    audio_init();
}

void PicoAudio::loop() {
    audio_callback(nullptr, reinterpret_cast<uint8_t *>(m_stream), 1098);
    i2s_dma_write(&m_i2s_config, reinterpret_cast<const int16_t *>(m_stream));
}
