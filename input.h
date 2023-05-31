//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_INPUT_H
#define MICROBOY_INPUT_H

#include <cstdint>
#include "clock.h"

#ifndef BIT
#define BIT(n) (1U<<(n))
#endif

#define MAX_BUTTONS 8

namespace mb {
    class Input {
    public:
        enum Button {
            B1 = BIT(0),
            B2 = BIT(1),
            START = BIT(2),
            SELECT = BIT(3),
            LEFT = BIT(4),
            RIGHT = BIT(5),
            UP = BIT(6),
            DOWN = BIT(7),
            DELAY = BIT(8),
            QUIT = BIT(9)
        };

        virtual uint16_t getButtons();

        uint16_t getRepeatDelay() { return m_repeatDelayMs; };

        void setRepeatDelay(uint16_t ms) { m_repeatDelayMs = ms; };

    protected:
        Clock m_repeatClock{};
        uint16_t m_buttons{0};
        uint16_t m_buttons_prev{0};
        uint16_t m_repeatDelayMs = 0;
    };
}

#endif //MICROBOY_INPUT_H
