//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_INPUT_PICO_H
#define MICROBOY_INPUT_PICO_H

#include "input.h"

namespace mb {
    class PicoInput : public Input {
    public:
        PicoInput();

        uint16_t getButtons() override;
    };
}

#endif //MICROBOY_INPUT_PICO_H
