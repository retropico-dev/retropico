//
// Created by cpasjuste on 26/02/24.
//

#include "retropico_overlay.h"

using namespace p2d;
using namespace retropico;

Overlay::Overlay(Config *config, Platform *platform, const Utility::Vec4i &bounds) {
    Overlay::setPosition(bounds.x, bounds.y);
    Overlay::setSize(bounds.w, bounds.h);

    p_platform = platform;

    // apply config
    p_config = config;
    platform->getAudio()->setVolume(p_config->getVolume());

    // volume slider
    p_volume_widget = new VolumeWidget({
        static_cast<int16_t>(bounds.w - 6), 32,
        12, static_cast<int16_t>(bounds.h - 64)
    });

    p_volume_widget->setVolume(platform->getAudio()->getVolume());
    p_volume_widget->setOrigin(Origin::TopRight);
    Overlay::add(p_volume_widget);

    Overlay::setVisibility(Visibility::Hidden);
}

void Overlay::onUpdate(p2d::Time delta) {
    auto buttons = p_platform->getInput()->getRawButtons();
    bool changed = (m_buttons_old ^ buttons) != 0;
    m_buttons_old = buttons;

    if (changed || m_input_clock.getElapsedTime().asMilliseconds() > 300) {
        // volume keys
        if (buttons & p2d::Input::Button::VOL_UP) {
            setVisibility(Visibility::Visible);
            m_visibility_clock.restart();
            uint8_t volume = p_platform->getAudio()->getVolume();
            uint8_t volumeMax = p_platform->getAudio()->getVolumeMax();
            if (volume < volumeMax) {
                p_platform->getAudio()->setVolume(volume + 10);
                p_volume_widget->setVolume(p_platform->getAudio()->getVolume());
            }
        } else if (buttons & p2d::Input::Button::VOL_DOWN) {
            setVisibility(Visibility::Visible);
            m_visibility_clock.restart();
            uint8_t volume = p_platform->getAudio()->getVolume();
            if (volume > 0) {
                p_platform->getAudio()->setVolume(volume - 10);
                p_volume_widget->setVolume(p_platform->getAudio()->getVolume());
            }
        }
        m_input_clock.restart();
    }

    if (isVisible()) {
        Widget::onUpdate(delta);
        if (m_visibility_clock.getElapsedTime().asMilliseconds() > 2000) {
            setVisibility(Visibility::Hidden);
        }
    }
}
