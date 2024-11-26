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
#include "retropico_overlay.h"

using namespace retropico;
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
#elif MB_SAMEBOY
static Display::Settings ds{
    .displaySize = {240, 240},
    .renderSize = {240, 240},
    .renderBounds = {0, 0, 240, 240},
    .bufferingMode = Display::Buffering::None,
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
    auto core = new MBCore(ds);

    // retrieve rom from flash
#ifndef NDEBUG
    Io::File file{"res:/romfs/rom.bin"};
#else
    auto list = Io::getList("flash:/rom/");
    if (list.empty()) {
        core->getOverlay()->getInfoBox()->show("OOPS: ROM COULD NOT BE LOADED...", 1000 * 5, core);
        // reboot to ui
        core->reboot(FLASH_MAGIC_UI);
    }
    Io::File file{"flash:/rom/" + list[0].name};
#endif

    // load rom
    if (!core->loadRom(file)) {
        core->getOverlay()->getInfoBox()->show("OOPS: ROM COULD NOT BE LOADED...", 1000 * 5, core);
        // reboot to ui
        core->reboot(FLASH_MAGIC_UI);
        return 1;
    }

    // emulation loop
    while (core->loop()) {
    }

    // save...
    core->getConfig()->save();

    // cleanly close core (handle saves and such)
    core->close();

    // reboot to ui
    core->reboot(FLASH_MAGIC_UI);

    // unreachable
    delete (core);

    return 0;
}
