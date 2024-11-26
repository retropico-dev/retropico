//
// Created by cpasjuste on 26/02/24.
//

#include "retropico_overlay.h"
#include "retropico_colors.h"

using namespace p2d;
using namespace retropico;

// TODO: move config stuff to libpico2d ?

Overlay::Overlay(Config *config, Platform *platform, const Utility::Vec4i &bounds) {
    Overlay::setPositionAndSize(bounds.x, bounds.y, bounds.w, bounds.h);

    p_platform = platform;

    // apply config
    p_config = config;
    platform->getAudio()->setVolume(p_config->getVolume());

    // infobox
    const auto size = Overlay::getSize();
    p_infoBox = new InfoBox({(int16_t) (size.x / 2), (int16_t) (size.y - 2)},
                            {(int16_t) (size.x - 4), 16},
                            Black, YellowLight);
    p_infoBox->setOrigin(Origin::Bottom);
    p_infoBox->setVisibility(Visibility::Hidden);
    Overlay::add(p_infoBox);

    // volume slider
    p_volume_widget = new VolumeWidget({
        static_cast<int16_t>(bounds.w - 6), 32,
        12, static_cast<int16_t>(bounds.h - 64)
    });
    platform->getAudio()->setVolume(config->getVolume());
    p_volume_widget->setVolume(platform->getAudio()->getVolume());
    p_volume_widget->setOrigin(Origin::TopRight);
    p_volume_widget->setVisibility(Visibility::Hidden);
    Overlay::add(p_volume_widget);
}

void Overlay::onUpdate(Time delta) {
    auto buttons = p_platform->getInput()->getRawButtons();
    bool changed = (m_buttons_old ^ buttons) != 0;
    m_buttons_old = buttons;

    if (changed || m_input_clock.getElapsedTime().asMilliseconds() > 300) {
        // volume keys
        if (buttons & Input::Button::VOL_UP) {
            p_volume_widget->setVisibility(Visibility::Visible);
            m_visibility_clock.restart();
            const auto volume = p_platform->getAudio()->getVolume();
            const auto volumeMax = p_platform->getAudio()->getVolumeMax();
            if (volume < volumeMax) {
                p_platform->getAudio()->setVolume(volume + 10);
                p_config->setVolume(p_platform->getAudio()->getVolume());
                p_volume_widget->setVolume(p_platform->getAudio()->getVolume());
            }
        } else if (buttons & Input::Button::VOL_DOWN) {
            p_volume_widget->setVisibility(Visibility::Visible);
            m_visibility_clock.restart();
            const auto volume = p_platform->getAudio()->getVolume();
            if (volume > 0) {
                p_platform->getAudio()->setVolume(volume - 10);
                p_config->setVolume(p_platform->getAudio()->getVolume());
                p_volume_widget->setVolume(p_platform->getAudio()->getVolume());
            }
        }
        m_input_clock.restart();
    }

    if (p_volume_widget->isVisible() && m_visibility_clock.getElapsedTime().asMilliseconds() > 2000) {
        p_volume_widget->setVisibility(Visibility::Hidden);
    }

    Widget::onUpdate(delta);
}
