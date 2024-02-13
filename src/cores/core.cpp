//
// Created by cpasjuste on 01/08/23.
//

#include "core.h"

using namespace p2d;
using namespace mb;

bool Core::loop() {
    // input
    Platform::onInput(0);
    uint16_t buttons = getInput()->getButtons();
    if (buttons & p2d::Input::Button::START && buttons & p2d::Input::Button::SELECT ||
        p_input->getButtons() & Input::Button::QUIT) {
        return false;
    }

    // volume up/down
    if (buttons & p2d::Input::Button::SELECT) {
        getInput()->setRepeatDelay(INPUT_DELAY_UI);
        if (buttons & p2d::Input::Button::UP) {
            getAudio()->volumeUp();
            printf("volume: %i\n", getAudio()->getVolume());
        } else if (buttons & p2d::Input::Button::DOWN) {
            getAudio()->volumeDown();
            printf("volume: %i\n", getAudio()->getVolume());
        }
    } else if (!(buttons & p2d::Input::Button::DELAY)) {
        getInput()->setRepeatDelay(0);
    }

    // update loop
    onUpdate(m_delta_clock.restart());

    // drawing is handled in emulation core
    //onDraw(m_position, true);

    return true;
}
