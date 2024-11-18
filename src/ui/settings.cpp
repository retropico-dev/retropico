//
// Created by cpasjuste on 04/07/23.
//

#include "main.h"
#include "settings.h"

using namespace p2d;
using namespace retropico;

Settings::Settings(const Utility::Vec2i &pos, const Utility::Vec2i &size)
        : Rectangle(pos, size, Gray, 8) {
    Settings::setColor(Display::Color::Transparent);

    auto text = new Text((int16_t) (size.x / 2), (int16_t) (size.y / 2), "COMING SOON !");
    text->setOrigin(Origin::Center);
    add(text);
}
