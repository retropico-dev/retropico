//
// Created by cpasjuste on 31/05/23.
//

#include <SDL2/SDL.h>
#include "audio_linux.h"

using namespace mb;

LinuxAudio::LinuxAudio() : Audio() {
    SDL_AudioSpec want, have;

    if (!SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_InitSubSystem(SDL_INIT_AUDIO);
    }

    want.freq = AUDIO_SAMPLE_RATE;
    want.format = AUDIO_S16;
    want.channels = 2;
    want.samples = AUDIO_SAMPLES;
    want.callback = audio_callback;
    want.userdata = nullptr;

    SDL_LogMessage(SDL_LOG_CATEGORY_AUDIO, SDL_LOG_PRIORITY_INFO,
                   "Audio driver: %s", SDL_GetAudioDeviceName(0, 0));

    if ((m_dev = SDL_OpenAudioDevice(nullptr, 0, &want, &have, 0)) == 0) {
        SDL_LogMessage(SDL_LOG_CATEGORY_ERROR, SDL_LOG_PRIORITY_CRITICAL,
                       "SDL could not open audio device: %s", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    audio_init();
    SDL_PauseAudioDevice(m_dev, 0);
}

LinuxAudio::~LinuxAudio() {
    if (m_dev != 0u) {
        SDL_PauseAudioDevice(m_dev, 1);
        SDL_CloseAudioDevice(m_dev);
    }

    if (SDL_WasInit(SDL_INIT_AUDIO)) {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
    }
}
