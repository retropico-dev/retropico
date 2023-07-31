//
// Created by cpasjuste on 31/05/23.
//

#ifndef MICROBOY_INPUT_H
#define MICROBOY_INPUT_H

#include <cstdint>
#include <string>
#include "clock.h"

#ifndef BIT
#define BIT(n) (1U<<(n))
#endif

#define MAX_BUTTONS 8
#define INPUT_DELAY_UI 300

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

        struct Mapping {
            uint8_t button{};
            int8_t pin{};
            std::string name{};
        };

        virtual uint16_t getButtons();

        uint16_t getRepeatDelay() { return m_repeatDelayMs; };

        void setRepeatDelay(uint16_t ms) { m_repeatDelayMs = ms; };

        Mapping *getMapping() { return m_mapping; };

    protected:
        Clock m_repeatClock{};
        uint16_t m_buttons{0};
        uint16_t m_buttons_prev{0};
        uint16_t m_repeatDelayMs = INPUT_DELAY_UI;
        Mapping m_mapping[MAX_BUTTONS];
    };
}

#endif //MICROBOY_INPUT_H
