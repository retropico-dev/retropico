//
// Created by cpasjuste on 31/05/23.
//

#include <cstring>
#include <hardware/pio.h>
#include "platform.h"
#include "pinout.h"

#define audio_pio __CONCAT(pio, PICO_AUDIO_I2S_PIO)

using namespace mb;

void PicoAudio::setup(uint16_t rate, uint16_t samples, uint8_t channels) {
    Audio::setup(rate, samples, channels);
    printf("PicoAudio::setup: rate: %i, samples: %i, channels: %i\r\n",
           m_rate, m_samples, m_channels);

    static audio_format_t audio_format = {
            .sample_freq = rate,
            .format = AUDIO_BUFFER_FORMAT_PCM_S16,
            .channel_count = m_channels,
    };

    static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = (uint16_t) (m_channels * 2)
    };

    uint8_t dma_channel = dma_claim_unused_channel(true);
    uint8_t pio_sm = pio_claim_unused_sm(audio_pio, true);

    // audio_i2s_setup claims
    dma_channel_unclaim(dma_channel);
    pio_sm_unclaim(audio_pio, pio_sm);

    m_i2s_config = {
            .data_pin = AUDIO_PIN_DATA,
            .clock_pin_base = AUDIO_PIN_CLOCK,
            .dma_channel = dma_channel,
            .pio_sm = pio_sm
    };

    p_producer_pool = audio_new_producer_pool(&producer_format, 3, samples);
    const struct audio_format *output_format = audio_i2s_setup(&audio_format, &m_i2s_config);
    if (!output_format) {
        panic("PicoAudio::setup: audio_i2s_setup failed\n");
    }

    bool ret = audio_i2s_connect(p_producer_pool);
    if (!ret) {
        panic("PicoAudio::setup: audio_i2s_connect_extra failed\r\n");
    }

    audio_i2s_set_enabled(true);

    printf("\r\n");
}

void in_ram(PicoAudio::play)(const void *data, int samples) {
    auto buffer = take_audio_buffer(p_producer_pool, true);
    auto sampleBuffer = (int16_t *) buffer->buffer->bytes;
    if (m_volume == 100) {
        memcpy(sampleBuffer, data, samples * sizeof(int16_t) * m_channels);
    } else {
        auto dataBuffer = (int16_t *) data;
        for (uint_fast16_t i = 0; i < (uint_fast16_t) samples * m_channels; i++) {
            //sampleBuffer[i] = (int16_t) (dataBuffer[i] >> m_volume);
            sampleBuffer[i] = (int16_t) ((dataBuffer[i] * m_volume) / 100);
        }
    }

    buffer->sample_count = samples;
    give_audio_buffer(p_producer_pool, buffer);
}
