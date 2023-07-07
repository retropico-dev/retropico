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

    // add menu
    p_menu = new Menu({-8, (int16_t) (Ui::getSize().y / 2)}, {150, 144});
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

    // only refresh screen on button inputs
    if (force || buttons && buttons != Input::Button::DELAY) {
        if (buttons & Input::Button::SELECT) {
            p_menu->setVisibility(p_menu->isVisible() ? Visibility::Hidden : Visibility::Visible);
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
