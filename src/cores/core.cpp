//
// Created by cpasjuste on 01/08/23.
//

#include "core.h"

using namespace p2d;
using namespace mb;

bool Core::loop() {
    // update inputs
    Platform::onInput(0);

    // check for quit
    uint16_t buttons = getInput()->getButtons();
    if (buttons & p2d::Input::Button::START && buttons & p2d::Input::Button::SELECT ||
        p_input->getButtons() & Input::Button::QUIT) {
        return false;
    }

    // update loop
    onUpdate(m_delta_clock.restart());

    // drawing is handled in emulation core
    //onDraw(m_position, true);

    return true;
}
