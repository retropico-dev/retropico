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
#include "smsplus.h"

using namespace mb;

int main() {
    Clock clock;
    int frames = 0;

    auto platform = new MBPlatform();
    auto core = new SMSPlus(platform);

#ifndef LINUX
    Io::FileBuffer fb = platform->getIo()->readRomFromFlash();
    if (!core->loadRom(fb)) {
        // reboot to ui
        platform->reboot(Platform::RebootTarget::Ui);
    }
#else
    if (!core->loadRom("roms/sms/Alex Kidd in Miracle World (USA, Europe, Brazil) (Rev 1).sms")) {
        // reboot to ui
        platform->reboot(Platform::RebootTarget::Ui);
    }
#endif

    // emulation loop
    while (core->loop()) {
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

    // reboot to ui
    platform->reboot(Platform::RebootTarget::Ui);

    // unreachable
    delete (core);
    delete (platform);

    return 0;
}
