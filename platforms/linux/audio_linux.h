//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_AUDIO_LINUX_H
#define MICROBOY_AUDIO_LINUX_H

#include "audio.h"

namespace mb {
    class LinuxAudio : public Audio {
    public:
        explicit LinuxAudio(AudioCallback cb = nullptr);

        ~LinuxAudio() override;

    private:
        SDL_AudioDeviceID m_dev;
    };
}

#endif //MICROBOY_AUDIO_LINUX_H
