//
// Created by cpasjuste on 01/08/23.
//

#include "core.h"

using namespace p2d;
using namespace retropico;

Core::Core(const Display::Settings &displaySettings, const Core::Type &core) : P2DPlatform(displaySettings) {
    p_config = new Config();
    p_audio->setVolume(p_config->getVolume());

    // add overlay
    p_overlay = new Overlay(p_config, this, Core::getBounds());
    Core::add(p_overlay);
}

bool Core::loop() {
    // update inputs
    Platform::onInput(0);

    // check for quit
    uint16_t buttons = getInput()->getButtons();
    if (buttons & Input::Button::START && buttons & Input::Button::SELECT ||
        p_input->getButtons() & Input::Button::QUIT) {
        return false;
    }

    // update loop
    onUpdate(m_delta_clock.restart());

    // drawing is handled in emulated cores
    //onDraw(m_position, true);

    return true;
}
