//
// Created by cpasjuste on 04/07/23.
//

#include "platform.h"
#include "settings.h"
#include "ui.h"

using namespace mb;

Settings::Settings(const Utility::Vec2i &pos, const Utility::Vec2i &size)
        : Rectangle(pos, size, Ui::Color::Gray, 8) {
    Settings::setColor(Ui::Color::Transparent);

    auto text = new Text((int16_t) (size.x / 2), (int16_t) (size.y / 2), "COMING SOON !");
    text->setOrigin(Origin::Center);
    add(text);
}

void Settings::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;
    Rectangle::loop(pos, buttons);
}
