//
// Created by cpasjuste on 26/05/23.
//

#ifdef LINUX
#include <ctime>
#include <sys/time.h>
#else
#include <pico/time.h>
#endif
#include "clock.h"

using namespace mb;

Clock::Clock() {
    m_startTime = Clock::getCurrentTime();
}

Time Clock::getCurrentTime() const {
#ifdef LINUX
    timespec time = {};
    clock_gettime(CLOCK_MONOTONIC, &time);
    return microseconds((long) time.tv_sec * 1000000 + time.tv_nsec / 1000);
#else
    return microseconds((long) to_us_since_boot(get_absolute_time()));
#endif
}

Time Clock::getElapsedTime() const {
    return getCurrentTime() - m_startTime;
}

Time Clock::restart() {
    Time now = getCurrentTime();
    Time elapsed = now - m_startTime;
    m_startTime = now;
    return elapsed;
}
