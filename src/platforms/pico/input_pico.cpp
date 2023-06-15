//
// Created by cpasjuste on 31/05/23.
//

#include "platform.h"
#include "pinout.h"

using namespace mb;

struct Mapping {
    uint8_t button;
    int8_t pin;
};

const Mapping mapping[MAX_BUTTONS] = {
        {Input::Button::B1,     BTN_PIN_B1},
        {Input::Button::B2,     BTN_PIN_B2},
        {Input::Button::START,  BTN_PIN_START},
        {Input::Button::SELECT, BTN_PIN_SELECT},
        {Input::Button::LEFT,   BTN_PIN_LEFT},
        {Input::Button::RIGHT,  BTN_PIN_RIGHT},
        {Input::Button::UP,     BTN_PIN_UP},
        {Input::Button::DOWN,   BTN_PIN_DOWN}
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
