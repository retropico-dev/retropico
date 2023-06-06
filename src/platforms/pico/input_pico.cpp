//
// Created by cpasjuste on 31/05/23.
//

#include "platform.h"

using namespace mb;

struct Mapping {
    uint8_t button;
    int8_t pin;
};

const Mapping mapping[MAX_BUTTONS] = {
        {Input::Button::B1,     -1},
        {Input::Button::B2,     -1},
        {Input::Button::START,  29},
        {Input::Button::SELECT, -1},
        {Input::Button::LEFT,   -1},
        {Input::Button::RIGHT,  -1},
        {Input::Button::UP,     -1},
        {Input::Button::DOWN,   -1}
};

PicoInput::PicoInput() : Input() {
    for (const auto &map: mapping) {
        if (map.pin != -1) {
            gpio_set_function(map.pin, GPIO_FUNC_SIO);
            gpio_set_dir(map.pin, false);
            gpio_pull_up(map.pin);
        }
    }
}

uint16_t PicoInput::getButtons() {
    // reset buttons state
    m_buttons = 0;

    // check for buttons (keyboard) press
    for (const auto &map: mapping) {
        if (map.pin != -1) m_buttons |= gpio_get(map.pin) ? 0 : map.button;
    }

    // handle repeat delay
    return Input::getButtons();
}
