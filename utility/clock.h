//
// Created by cpasjuste on 26/05/23.
//

#ifndef TAMAGOPICO_CLOCK_H
#define TAMAGOPICO_CLOCK_H

#include "clock_time.h"

namespace mb {
    class Clock {
    public:
        Clock();

        [[nodiscard]] Time getCurrentTime() const;

        [[nodiscard]] Time getElapsedTime() const;

        Time restart();

    protected:
        Time m_startTime;
    };
}

#endif //TAMAGOPICO_CLOCK_H
