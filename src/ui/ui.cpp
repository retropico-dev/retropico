//
// Created by cpasjuste on 14/06/23.
//

#include "platform.h"
#include "ui.h"

using namespace p2d;
using namespace mb;

//...
static Ui *s_ui = nullptr;

Ui::Ui(Platform *platform) : Rectangle({1, 1},
                                       {(int16_t) (platform->getDisplay()->getSize().x - 2),
                                        (int16_t) (platform->getDisplay()->getSize().y - 2)},
                                       Display::Color::Transparent) {
    s_ui = this;
    p_platform = platform;

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);

    // set colors
    Ui::setOutlineThickness(1);
    Ui::setOutlineColor(Ui::Color::Red);

    // create needed directories
    Io::create(Core::getRomPath(Core::Type::Nes));
    Io::create(Core::getRomPath(Core::Type::Gb));
    Io::create(Core::getRomPath(Core::Type::Sms));
    Io::create(Core::getSavePath(Core::Type::Nes));
    Io::create(Core::getSavePath(Core::Type::Gb));
    Io::create(Core::getSavePath(Core::Type::Sms));

    // add filer
    p_filer = new Filer({1, 1}, {(int16_t) (Ui::getSize().x - 2), (int16_t) (Ui::getSize().y - 2)});
    Ui::add(p_filer);

    // add settings
    p_settings = new Settings(p_filer->getPosition(), p_filer->getSize());
    p_settings->setVisibility(Visibility::Hidden);
    Ui::add(p_settings);

    // add menu
    p_menu = new Menu({-1, (int16_t) (Ui::getSize().y / 2)}, {150 / 2, 192 / 2});
    p_menu->setOrigin(Origin::Left);
    p_menu->setVisibility(Visibility::Hidden);
    Ui::add(p_menu);

    // add info box
    p_infoBox = new InfoBox({(int16_t) (Ui::getSize().x / 2), (int16_t) (Ui::getSize().y / 2)},
                            {(int16_t) (Ui::getSize().x - 32), 128 - 64});
    p_infoBox->setOrigin(Origin::Center);
    p_infoBox->setVisibility(Visibility::Hidden);
    Ui::add(p_infoBox);

    // load files
    p_filer->setVisibility(Visibility::Hidden);
    p_infoBox->show("PLEASE WAIT");
    // force flip/show
    Rectangle::loop(m_position, 0);
    p_platform->getDisplay()->flip();
    p_filer->load();
    p_infoBox->hide();
    p_filer->setVisibility(Visibility::Visible);
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
            if (m_repeat_clock.getElapsedTime().asSeconds() > 6) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 20);
            } else if (m_repeat_clock.getElapsedTime().asSeconds() > 4) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 8);
            } else if (m_repeat_clock.getElapsedTime().asSeconds() > 2) {
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
