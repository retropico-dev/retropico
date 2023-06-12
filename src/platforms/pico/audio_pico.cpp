//
// Created by cpasjuste on 31/05/23.
//

#include "platform.h"

using namespace mb;

PicoAudio::PicoAudio() : Audio() {
}

void PicoAudio::setup(uint16_t rate, uint16_t samples, Audio::AudioCallback cb) {
    Audio::setup(rate, samples, cb);

    m_i2s_config = {
            .data_pin = 26,
            .clock_pin_base = 27,
            .dma_channel = 0,
            .pio_sm = 1
    };

    static audio_format_t audio_format = {
            .sample_freq = rate,
            .format = AUDIO_BUFFER_FORMAT_PCM_S16,
            .channel_count = 2,
    };

    static struct audio_buffer_format producer_format = {
            .format = &audio_format,
            .sample_stride = 4
    };

    p_producer_pool = audio_new_producer_pool(&producer_format, 2, samples);
    const struct audio_format *output_format = audio_i2s_setup(&audio_format, &m_i2s_config);
    if (!output_format) {
        panic("PicoAudio::setup: Unable to open audio device.\n");
    }

    bool ret = audio_i2s_connect_extra(p_producer_pool, false, 2, samples, nullptr);
    if (!ret) {
        panic("PicoAudio::setup: audio_i2s_connect_extra failed\r\n");
    }

    audio_i2s_set_enabled(true);
}

void PicoAudio::play(const void *data, int samples) {
    auto buffer = take_audio_buffer(p_producer_pool, true);
    auto sampleBuffer = (uint16_t *) buffer->buffer->bytes;
    if (m_volume == 0) {
        memcpy(sampleBuffer, data, samples * sizeof(int32_t));
    } else {
        auto dataBuffer = (int16_t *) data;
        for (uint16_t i = 0; i < (uint16_t) samples * 2; i++) {
            sampleBuffer[i] = dataBuffer[i] >> m_volume;
        }
    }
    buffer->sample_count = samples;
    give_audio_buffer(p_producer_pool, buffer);
}
