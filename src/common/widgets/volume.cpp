//
// Created by cpasjuste on 26/02/24.
//

#include "volume.h"

using namespace p2d;
using namespace retropico;

VolumeWidget::VolumeWidget(const p2d::Utility::Vec4i &bounds) : Rectangle(bounds) {
    setColor(Display::Color::GrayDark);
    setOutlineColor(Display::Color::Red);
    setOutlineThickness(1);

    p_slider_rect = new Rectangle(1, getSize().y - 1, getSize().x - 2, getSize().y - 2);
    p_slider_rect->setOrigin(Origin::BottomLeft);
    p_slider_rect->setColor(Display::Color::Yellow);
    add(p_slider_rect);

    p_text = new Text(getSize().x / 2, -2, "80", Display::Color::Yellow);
    p_text->setOrigin(Origin::Bottom);
    add(p_text);
}

void VolumeWidget::setVolume(uint8_t volume) {
    int16_t height = (int16_t) (volume * (getSize().y - 2) / 100);
    if (height > 0) {
        p_slider_rect->setVisibility(Visibility::Visible);
        p_slider_rect->setSize(p_slider_rect->getSize().x, (int16_t) (volume * (getSize().y - 2) / 100));
        p_text->setString(std::to_string(volume));
    } else {
        p_slider_rect->setVisibility(Visibility::Hidden);
        p_text->setString("00");
    }
}
