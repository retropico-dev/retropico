//
// Created by cpasjuste on 02/08/23.
//

#include "main.h"
#include "infobox.h"

using namespace p2d;
using namespace retropico;

InfoBox::InfoBox(const Utility::Vec2i &pos, const Utility::Vec2i &size)
        : Rectangle(pos, size, Ui::Color::GrayDark, 8) {
    InfoBox::setOutlineColor(Ui::Color::Red);
    InfoBox::setOutlineThickness(1);
    p_text = new Text((int16_t) (size.x / 2), (int16_t) (size.y / 2), "InfoBox !");
    p_text->setSize(size.x - 6, 0);
    p_text->setColor(Ui::Color::Yellow);
    p_text->setOrigin(Origin::Center);
    add(p_text);
    m_clock.restart();
}

void InfoBox::show(const std::string &text, uint32_t millis) {
    p_text->setString(text);
    m_clock.restart();
    m_millis = millis;
    setVisibility(Visibility::Visible);
    InfoBox::onDraw({static_cast<int16_t>(getPosition().x - getSize().x / 2),
                     static_cast<int16_t>(getPosition().y - getSize().y / 2)}, true);
    Ui::getInstance()->getPlatform()->getDisplay()->flip();
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
