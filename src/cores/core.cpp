//
// Created by cpasjuste on 01/08/23.
//

#include "core.h"

using namespace mb;

bool in_ram(Core::loop)(uint16_t buttons) {
    // exit requested
    if (buttons & p2d::Input::Button::START && buttons & p2d::Input::Button::SELECT
        || buttons & p2d::Input::Button::QUIT) {
        return false;
    }

    // volume up/down
    if (buttons & p2d::Input::Button::SELECT) {
        p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
        if (!(buttons & p2d::Input::Button::DELAY)) {
            if (buttons & p2d::Input::Button::UP) {
                p_platform->getAudio()->volumeUp();
                printf("volume: %i\n", p_platform->getAudio()->getVolume());
            } else if (buttons & p2d::Input::Button::DOWN) {
                p_platform->getAudio()->volumeDown();
                printf("volume: %i\n", p_platform->getAudio()->getVolume());
            }
        }
    } else if (!(buttons & p2d::Input::Button::DELAY)) {
        p_platform->getInput()->setRepeatDelay(0);
    }

    return true;
}
