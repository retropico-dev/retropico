//
// Created by cpasjuste on 31/05/23.
//

#include "platform.h"

using namespace mb;

struct Mapping {
    uint8_t button;
    uint8_t scancode;
};

const Mapping mapping[MAX_BUTTONS] = {
        {Input::Button::B1,     SDL_SCANCODE_KP_1},
        {Input::Button::B2,     SDL_SCANCODE_KP_2},
        {Input::Button::START,  SDL_SCANCODE_RETURN},
        {Input::Button::SELECT, SDL_SCANCODE_SPACE},
        {Input::Button::LEFT,   SDL_SCANCODE_LEFT},
        {Input::Button::RIGHT,  SDL_SCANCODE_RIGHT},
        {Input::Button::UP,     SDL_SCANCODE_UP},
        {Input::Button::DOWN,   SDL_SCANCODE_DOWN}
};

uint16_t LinuxInput::getButtons() {
    // reset buttons state
    m_buttons = 0;

    // check for quit event
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if (ev.type == SDL_QUIT) {
            return Input::Button::QUIT;
        }
    }

    // check for buttons (keyboard) press
    for (const auto &map: mapping) {
        m_buttons |= SDL_GetKeyboardState(nullptr)[map.scancode] > 0 ? map.button : 0;
    }

    // handle repeat delay
    return Input::getButtons();
}
