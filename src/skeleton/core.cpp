//
// Created by cpasjuste on 01/08/23.
//

#include "core.h"

using namespace mb;

bool Core::loop(uint16_t buttons) {
    // exit requested
    if (buttons & Input::Button::START && buttons & Input::Button::SELECT
        || buttons & Input::Button::QUIT) {
        return false;
    }

    // volume up/down
    if (buttons & Input::Button::SELECT) {
        p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
        if (!(buttons & Input::Button::DELAY)) {
            if (buttons & Input::Button::UP) {
                p_platform->getAudio()->volumeUp();
                printf("volume: %i\n", p_platform->getAudio()->getVolume());
            } else if (buttons & Input::Button::DOWN) {
                p_platform->getAudio()->volumeDown();
                printf("volume: %i\n", p_platform->getAudio()->getVolume());
            }
        }
    } else if (!(buttons & Input::Button::DELAY)) {
        p_platform->getInput()->setRepeatDelay(0);
    }

    return true;
}
