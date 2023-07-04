//
// Created by cpasjuste on 14/06/23.
//

#include "platform.h"
#include "ui.h"

using namespace mb;

//...
mb::Platform *s_platform;

Ui::Ui(Platform *platform) : Rectangle({0, 0}, platform->getDisplay()->getSize()) {
    p_platform = s_platform = platform;

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);

    // set outline color
    Ui::setColor(Ui::Color::Red);

    // add filer
    p_filer = new Filer({1, 1}, {(int16_t) (Ui::getSize().x - 2), (int16_t) (Ui::getSize().y - 2)});
    Ui::add(p_filer);

    // first flip
    p_platform->getDisplay()->clear();
    Rectangle::loop(m_position, 0);
    p_platform->getDisplay()->flip();
}

bool Ui::loop() {
    uint16_t buttons = p_platform->getInput()->getButtons();
    if (buttons & Input::Button::QUIT) return false;

    // only refresh screen on button inputs
    if (buttons && buttons != Input::Button::DELAY) {
        // clear screen
        p_platform->getDisplay()->clear();

        // draw child's
        Rectangle::loop(m_position, buttons);

        // flip screen
        p_platform->getDisplay()->flip();
    }

    return true;
}

Platform *Ui::getPlatform() {
    return s_platform;
}

Display *Ui::getDisplay() {
    return s_platform->getDisplay();
}
