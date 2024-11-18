//
// Created by cpasjuste on 02/08/23.
//

#include "infobox.h"

using namespace p2d;
using namespace retropico;

InfoBox::InfoBox(const Utility::Vec2i &pos, const Utility::Vec2i &size,
                 const uint16_t &color, const uint16_t &bg_color)
    : Rectangle(pos, size, color) {
    InfoBox::setOutlineColor(bg_color);
    InfoBox::setOutlineThickness(1);
    p_text = new Text((int16_t) (size.x / 2), (int16_t) (size.y / 2), "InfoBox !");
    p_text->setSize(size.x - 6, 0);
    p_text->setColor(bg_color);
    p_text->setOrigin(Origin::Center);
    InfoBox::add(p_text);
    m_clock.restart();
}

void InfoBox::show(const std::string &text, uint32_t millis, Platform *platform) {
    p_text->setString(text);
    m_clock.restart();
    m_millis = millis;
    setVisibility(Visibility::Visible);
    if (platform) {
        onDraw(true);
        platform->getDisplay()->flip();
    }
}

void InfoBox::hide() {
    setVisibility(Visibility::Hidden);
}

bool InfoBox::onInput(const uint16_t &buttons) {
    if (m_millis > 0 && m_clock.getElapsedTime().asMilliseconds() >= m_millis) {
        setVisibility(Visibility::Hidden);
        return true;
    }

    return Rectangle::onInput(buttons);
}
