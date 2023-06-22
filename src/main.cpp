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
#include "ui.h"
#ifdef MB_GB
#include "peanut-gb.h"
#define MBCore PeanutGB
#elif MB_NES
#include "infones.h"
#define MBCore InfoNES
#endif

using namespace mb;

int main() {
    Clock clock;
    int frames = 0;

    auto platform = new MBPlatform();
    auto core = new MBCore(platform);
    auto ui = new Ui(platform);

    // if no valid rom was found in flash load ui
    if (!platform->getIo()->isRomInFlash()) {
        // main loop
        while (ui->loop()) {
            // fps
            if (clock.getElapsedTime().asSeconds() >= 1) {
                auto percent = (uint16_t) (((float) Utility::getUsedHeap() / (float) Utility::getTotalHeap()) * 100);
                printf("fps: %i, heap: %i/%i (%i free - %i%%)\r\n",
                       (int) ((float) frames / clock.restart().asSeconds()),
                       Utility::getUsedHeap(), Utility::getTotalHeap(), Utility::getFreeHeap(), percent);
                frames = 0;
            }

            // increment frames for fps counter
            frames++;
        }
        if (!core->loadRom(ui->getRom())) {
            stdio_flush();
            while (true) { __wfi(); }
        }
    } else {
        Io::FileBuffer buffer{
                .data = (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET_ROM_DATA),
                .size = 1024 * 1024
        };
        if (!core->loadRom(buffer)) {
            stdio_flush();
            while (true) { __wfi(); }
        }
    }

    // emulation loop
    while (true) {
        if (!core->loop()) {
            if (!ui->loop()) {
                if (!core->loadRom(ui->getRom())) {
                    stdio_flush();
                    while (true) { __wfi(); }
                }
            }
        }

        // fps
        if (clock.getElapsedTime().asSeconds() >= 1) {
            auto percent = (uint16_t) (((float) Utility::getUsedHeap() / (float) Utility::getTotalHeap()) * 100);
            printf("fps: %i, heap: %i/%i (%i free - %i%%)\r\n",
                   (int) ((float) frames / clock.restart().asSeconds()),
                   Utility::getUsedHeap(), Utility::getTotalHeap(), Utility::getFreeHeap(), percent);
            frames = 0;
        }

        // increment frames for fps counter
        frames++;
    }

    //delete (ui);
    delete (platform);

    return 0;
}
