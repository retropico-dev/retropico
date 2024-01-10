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

#include "platform.h"
#include "main.h"

using namespace mb;
using namespace p2d;

int main() {
    Clock clock;
    int frames = 0;

    auto platform = new P2DPlatform(false);
    auto display = (Display *) new PicoDisplay({240, 240}, {240, 240});
    platform->setDisplay(display);
    auto core = new MBCore(platform);

    Io::FileBuffer fb = platform->getIo()->readRomFromFlash();
    if (!core->loadRom(fb)) {
        // reboot to ui
#warning "TODO: reboot"
        //platform->reboot(Platform::RebootTarget::Ui);
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
#warning "TODO: reboot"
    //platform->reboot(Platform::RebootTarget::Ui);

    // unreachable
    delete (platform);

    return 0;
}
