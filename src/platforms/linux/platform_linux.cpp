//
// Created by cpasjuste on 30/05/23.
//

#include "platform_linux.h"
#include "display_linux.h"
#include "input_linux.h"
#include "audio_linux.h"
#include "io_linux.h"

using namespace mb;

LinuxPlatform::LinuxPlatform() : Platform() {
    p_display = new LinuxDisplay();
    p_audio = new LinuxAudio();
    p_input = new LinuxInput();
    p_io = new LinuxIo();
}

LinuxPlatform::~LinuxPlatform() {
    printf("~LinuxPlatform()\n");
    SDL_Quit();
}
