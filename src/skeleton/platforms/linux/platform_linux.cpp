//
// Created by cpasjuste on 30/05/23.
//

#include "platform.h"

using namespace mb;

LinuxPlatform::LinuxPlatform(bool useDoubleBufferDisplay) : Platform() {
    p_display = new LinuxDisplay();
    p_audio = new LinuxAudio();
    p_input = new LinuxInput();
    p_io = new LinuxIo();
}

LinuxPlatform::~LinuxPlatform() {
    printf("~LinuxPlatform()\n");
    SDL_Quit();
}
