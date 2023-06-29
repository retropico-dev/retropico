//
// Created by cpasjuste on 31/05/23.
//

#include "platform.h"
#include "pinout.h"

using namespace mb;

void PicoAudio::setup(uint16_t rate, uint16_t samples, uint8_t channels) {
    Audio::setup(rate, samples, channels);
    printf("PicoAudio::setup: rate: %i, samples: %i, channels: %i\r\n",
           m_rate, m_samples, m_channels);

    m_i2s_config = {
            .data_pin = AUDIO_PIN_DATA,
            .clock_pin_base = AUDIO_PIN_CLOCK,
            .dma_channel = 2,
            .pio_sm = 1
    };

    static audio_format_t audio_format = {
            .sample_freq = rate,
            .format = AUDIO_BUFFER_FORMAT_PCM_S16,
            .channel_count = m_channels,
    };

    static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = (uint16_t) (m_channels * 2)
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
    if (m_volume == 0) {
        memcpy(sampleBuffer, data, samples * sizeof(int16_t) * m_channels);
    } else {
        auto dataBuffer = (int16_t *) data;
        for (uint_fast16_t i = 0; i < (uint_fast16_t) samples * m_channels; i++) {
            sampleBuffer[i] = (int16_t) (dataBuffer[i] >> m_volume);
        }
    }

    buffer->sample_count = samples;
    give_audio_buffer(p_producer_pool, buffer);
}
