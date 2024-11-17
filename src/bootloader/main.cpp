//
// Created by cpasjuste on 17/11/24.
//

#if 0

#include <platform.h>
#include <text.h>

#include "main.h"

using namespace p2d;

static Display::Settings ds{
    .displaySize = {240, 240}, // real display size (hardware size)
    .renderSize = {240, 240}, // rendering size (framebuffer size)
    .renderBounds = {0, 0, 240, 240}, // rendering bounds, used for scaling (if renderSize != renderBounds.w/h)
    .bufferingMode = Display::Buffering::Double,
    .format = Display::Format::RGB565
};

int main() {
    const auto platform = new P2DPlatform(ds);
    platform->getDisplay()->setClearColor(Display::Color::Black);

    const auto bounds = platform->getDisplay()->getSize();
    const auto text = new Text(static_cast<int16_t>(bounds.x - 2),
                               static_cast<int16_t>(bounds.y - 2),
                               "MicroBoy Bootloader...");
    text->setColor(Display::Color::Red);
    text->setOrigin(Widget::Origin::BottomRight);
    platform->add(text);

    for (int i = 0; i < 10; i++) {
        platform->loop();
    }

    return bootloader_main();
}
#endif
