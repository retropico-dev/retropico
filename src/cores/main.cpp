//
// Created by cpasjuste on 30/05/23.
//

/**
 * Copyright (C) 2023 by cpasjuste <cpasjuste@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "main.h"

using namespace mb;
using namespace p2d;

#ifdef MB_NES
static Display::Settings ds{
        .displaySize = {240, 240},
        .renderSize = {240, 240},
        .renderBounds = {0, 0, 240, 240},
        .bufferingMode = Display::Buffering::None,
        .format = Display::Format::ARGB444
};
#elif MB_GB
static Display::Settings ds{
        .displaySize = {240, 240},
        .renderSize = {160, 144},
        .renderBounds = {0, 0, 240, 240},
        .bufferingMode = Display::Buffering::Double,
        .format = Display::Format::RGB565
};
#elif MB_SMS
static Display::Settings ds{
        .displaySize = {240, 240},
        .renderSize = {240, 240},
        .bufferingMode = Display::Buffering::None,
        .format = Display::Format::RGB565
};
#endif

int main() {
    Clock clock;
    int frames = 0;

    auto platform = new P2DPlatform(MAX_OVERCLOCK);
    auto display = (Display *) new P2DDisplay(ds);
    platform->setDisplay(display);
    auto core = new MBCore(platform);

#ifndef NDEBUG
    Io::FileBuffer fb;
    RomFs::Binary binary = RomFs::get(ROMFS_ROM);
    fb.data = (uint8_t *) binary.data;
    fb.size = binary.size;
#else
    Io::FileBuffer fb = platform->getIo()->readRomFromFlash();
#endif
    if (!core->loadRom(fb)) {
        // reboot to ui
        platform->reboot(FLASH_MAGIC_UI);
    }

    // emulation loop
    while (true) {
        if (!core->loop(platform->getInput()->getButtons())) break;

        // fps
        if (clock.getElapsedTime().asSeconds() >= 1) {
            auto percent = (uint16_t) (((float) Utility::getUsedHeap() / (float) Utility::getTotalHeap()) * 100);
            printf("fps: %i, heap: %i/%i (%i%%)\r\n",
                   (int) ((float) frames / clock.restart().asSeconds()),
                   Utility::getUsedHeap(), Utility::getTotalHeap(), percent);
            frames = 0;
        }
        // increment frames for fps counter
        frames++;
    }

    // cleanly close core (handle saves and such)
    delete (core);

    // reboot to ui
    platform->reboot(FLASH_MAGIC_UI);

    // unreachable
    delete (platform);

    return 0;
}
