//
// Created by cpasjuste on 14/06/23.
//

#include "platform.h"
#include "ui.h"

using namespace mb;

//...
static Ui *s_ui = nullptr;

Ui::Ui(Platform *platform)
        : Rectangle({2, 2}, {(int16_t) (platform->getDisplay()->getSize().x - 4),
                             (int16_t) (platform->getDisplay()->getSize().y - 4)},
                    Color::Transparent, 8) {
    s_ui = this;
    p_platform = platform;

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);

    // set colors
    Ui::setColor(Color::Black);
    Ui::setOutlineThickness(2);
    Ui::setOutlineColor(Ui::Color::Red);

    // add filer
    p_filer = new Filer({1, 1}, {(int16_t) (Ui::getSize().x - 2), (int16_t) (Ui::getSize().y - 2)});
    Ui::add(p_filer);

    // add settings
    p_settings = new Settings(p_filer->getPosition(), p_filer->getSize());
    p_settings->setVisibility(Visibility::Hidden);
    Ui::add(p_settings);

    // add menu
    p_menu = new Menu({-8, (int16_t) (Ui::getSize().y / 2)}, {150, 192});
    p_menu->setOrigin(Origin::Left);
    p_menu->setVisibility(Visibility::Hidden);
    Ui::add(p_menu);

    // first flip
    p_platform->getDisplay()->clear();
    Rectangle::loop(m_position, 0);
    p_platform->getDisplay()->flip();
}

bool Ui::loop(bool force) {
    uint16_t buttons = p_platform->getInput()->getButtons();
    if (buttons & Input::Button::QUIT || p_filer->isDone()) return false;

    // handle auto-repeat speed
    if (buttons != Input::Button::DELAY) {
        bool changed = (m_buttons_old ^ buttons) != 0;
        m_buttons_old = buttons;
        if (!changed) {
            if (m_repeat_clock.getElapsedTime().asSeconds() > 5) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 20);
            } else if (m_repeat_clock.getElapsedTime().asSeconds() > 3) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 8);
            } else if (m_repeat_clock.getElapsedTime().asSeconds() > 1) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 4);
            }
        } else {
            p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
            m_repeat_clock.restart();
        }
    }

    // only refresh screen on button inputs
    if (force || buttons && buttons != Input::Button::DELAY) {
        if (buttons & Input::Button::SELECT && !p_menu->isVisible()) {
            p_menu->setVisibility(Visibility::Visible);
        }

        // clear screen
        p_platform->getDisplay()->clear();

        // draw child's
        Rectangle::loop(m_position, buttons);

        // flip screen
        p_platform->getDisplay()->flip();
    }

    return true;
}

Ui *Ui::getInstance() {
    return s_ui;
}

Platform *Ui::getPlatform() {
    return s_ui->p_platform;
}

Display *Ui::getDisplay() {
    return s_ui->p_platform->getDisplay();
}
