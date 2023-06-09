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
//#include "peanut-gb.h"
#include "infones.h"
//#include "gearsystem.h"

using namespace mb;

int main() {
    mb::Clock clock;
    int frames = 0;

    auto platform = new mb::MBPlatform();
    //auto core = new PeanutGB(platform);
    auto core = new InfoNES(platform);
    if (!core->loadRom("/roms/rom.gb")) {
        stdio_flush();
        while (true) { __wfi(); }
    }

    // emulation loop
    while (core->loop()) {
        // fps
        if (clock.getElapsedTime().asSeconds() >= 1) {
            printf("fps: %i\r\n", (int) ((float) frames / clock.restart().asSeconds()));
            frames = 0;
        }

        // increment frames for fps counter
        frames++;
    }

    delete (platform);

    return 0;
}
