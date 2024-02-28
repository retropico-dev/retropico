//
// Created by cpasjuste on 30/05/23.
//

/**
 * Copyright (C) 2023 by cpasjuste <cpasjuste@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "main.h"

using namespace p2d;
using namespace retropico;

static Display::Settings ds{
        .displaySize = {240, 240},
        .renderSize = {120, 120},
        .renderBounds = {0, 0, 240, 240},
        .bufferingMode = Display::Buffering::Double,
        .format = Display::Format::RGB565
};

//...
static Ui *s_ui = nullptr;

Ui::Ui(Platform *p) : Rectangle({1, 1},
                                {(int16_t) (p->getDisplay()->getSize().x - 2),
                                 (int16_t) (p->getDisplay()->getSize().y - 2)}) {
    s_ui = this;
    p_platform = p;

    // config
    p_config = new Config();

#ifdef NDEBUG
    // title/wait screen
    Clock titleClock;
#endif

    auto surface = new Surface(Io::File("res:/romfs/retropico.bmp"));
    p_platform->getDisplay()->drawSurface(surface);
    p_platform->getDisplay()->flip();
    delete (surface);

    // set repeat delay for ui
    p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);

    // set colors
    setColor(Color::GrayDark);
    setOutlineColor(Color::Red);
    setOutlineThickness(1);

    // create needed directories
    printf("Ui::Ui: creating needed directories\r\n");
    if (!Io::directoryExists("flash:/rom/")) Io::create("flash:/rom/");
    if (!Io::directoryExists(Core::getRomsPath(Core::Type::Nes))) Io::create(Core::getRomsPath(Core::Type::Nes));
    if (!Io::directoryExists(Core::getRomsPath(Core::Type::Gb))) Io::create(Core::getRomsPath(Core::Type::Gb));
    if (!Io::directoryExists(Core::getRomsPath(Core::Type::Sms))) Io::create(Core::getRomsPath(Core::Type::Sms));
    if (!Io::directoryExists(Core::getRomsPath(Core::Type::Gg))) Io::create(Core::getRomsPath(Core::Type::Gg));
    if (!Io::directoryExists(Core::getSavesPath(Core::Type::Nes))) Io::create(Core::getSavesPath(Core::Type::Nes));
    if (!Io::directoryExists(Core::getSavesPath(Core::Type::Gb))) Io::create(Core::getSavesPath(Core::Type::Gb));
    if (!Io::directoryExists(Core::getSavesPath(Core::Type::Sms))) Io::create(Core::getSavesPath(Core::Type::Sms));
    if (!Io::directoryExists(Core::getSavesPath(Core::Type::Gg))) Io::create(Core::getSavesPath(Core::Type::Gg));

    // add filer
    p_filer = new Filer({1, 1}, {(int16_t) (getSize().x - 2), (int16_t) (getSize().y - 2)});
    add(p_filer);

    // add settings
    p_settings = new Settings(p_filer->getPosition(), p_filer->getSize());
    p_settings->setVisibility(Visibility::Hidden);
    add(p_settings);

    // add menu
    p_menu = new Menu({-1, (int16_t) (getSize().y / 2)}, {150 / 2, 192 / 2});
    p_menu->setOrigin(Origin::Left);
    p_menu->setVisibility(Visibility::Hidden);
    add(p_menu);

    // add info box
    p_infoBox = new InfoBox({(int16_t) (getSize().x / 2), (int16_t) (getSize().y / 2)},
                            {(int16_t) (getSize().x - 32), 128 - 64});
    p_infoBox->setOrigin(Origin::Center);
    p_infoBox->setVisibility(Visibility::Hidden);
    add(p_infoBox);

    // load / cache files
    p_filer->load();

    // add overlay
    p_overlay = new Overlay(p_config, p_platform, getBounds());
    add(p_overlay);

#ifdef NDEBUG
    // give some time for title screen
    while (titleClock.getElapsedTime().asSeconds() < 1) tight_loop_contents();
#endif
}

void Ui::onUpdate(p2d::Time delta) {
    Rectangle::onUpdate(delta);

    // handle auto-repeat speed
    uint16_t buttons = p_platform->getInput()->getButtons();
    if (buttons != Input::Button::DELAY) {
        bool changed = (m_buttons_old ^ buttons) != 0;
        m_buttons_old = buttons;
        if (!changed) {
            if (m_repeat_clock.getElapsedTime().asSeconds() > 3) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 20);
            } else if (m_repeat_clock.getElapsedTime().asSeconds() > 2) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 8);
            } else if (m_repeat_clock.getElapsedTime().asSeconds() > 1) {
                p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI / 4);
            }
        } else {
            p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
            m_repeat_clock.restart();
        }
    }
}

bool Ui::onInput(const uint16_t &buttons) {
    if (buttons & Input::Button::QUIT || p_filer->isDone()) return false;

    if (buttons & Input::Button::SELECT && !p_menu->isVisible()) {
        p_menu->setVisibility(Visibility::Visible);
        return true;
    }

    return Widget::onInput(buttons);
}

Platform *Ui::getPlatform() {
    return p_platform;
}

Ui *Ui::getInstance() {
    return s_ui;
}

int main() {
    auto platform = new P2DPlatform(ds);
    auto ui = new Ui(platform);
    platform->add(ui);

    while (platform->loop() && !ui->isDone()) {}

    // save...
    ui->getConfig()->save();

    // reboot to bootloader for launching either nes/gb/sms core
    // based on rom header
    platform->reboot();

    // not really needed...
    delete (ui);
    delete (platform);

    return 0;
}
