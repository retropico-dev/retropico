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

        void setup(uint16_t rate, uint16_t samples, AudioCallback cb) override;

        void play(const void *data, int samples = 0) override;

    private:
        SDL_AudioDeviceID m_dev = 0;
    };
}

#endif //MICROBOY_AUDIO_LINUX_H
