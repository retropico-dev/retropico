//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_AUDIO_LINUX_H
#define MICROBOY_AUDIO_LINUX_H

namespace mb {
    class LinuxAudio : public Audio {
    public:
        explicit LinuxAudio();

        ~LinuxAudio() override;

        void setup(uint16_t rate, uint16_t samples) override;

        void play(const void *data, int samples) override;

    private:
        SDL_AudioDeviceID m_dev = 0;
        int16_t m_sampleBuffer[2048];
    };
}

#endif //MICROBOY_AUDIO_LINUX_H
