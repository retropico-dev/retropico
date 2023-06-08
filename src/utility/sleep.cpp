//
// Created by cpasjuste on 26/05/23.
//

#include <cstdio>
#ifndef LINUX
#include <pico/stdlib.h>
#include <pico/sleep.h>
#include <hardware/rosc.h>
#include <hardware/clocks.h>
#include <hardware/structs/scb.h>
#endif

#include "sleep.h"

static bool awake;

static void sleep_callback() {
    awake = true;
}

static void rtc_sleep() {
    awake = false;

#ifndef LINUX
    // Start on Friday 5th of June 2020 15:45:00
    datetime_t t = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 45,
            .sec   = 00
    };

    // Alarm 10 seconds later
    datetime_t t_alarm = {
            .year  = 2020,
            .month = 06,
            .day   = 05,
            .dotw  = 5, // 0 is Sunday, so 5 is Friday
            .hour  = 15,
            .min   = 45,
            .sec   = 5
    };

    // Start the RTC
    rtc_init();
    rtc_set_datetime(&t);

    printf("sleeping for 5 seconds\n");
    uart_default_tx_wait_blocking();
    sleep_goto_sleep_until(&t_alarm, &sleep_callback);
#endif
}

void rtc_recover(uint scb_orig, uint clock0_orig, uint clock1_orig) {
#ifndef LINUX
    // https://ghubcoder.github.io/posts/awaking-the-pico/

    // re-enable ring Oscillator control
    rosc_write(&rosc_hw->ctrl, ROSC_CTRL_ENABLE_BITS);

    // reset proc back to default
    scb_hw->scr = scb_orig;
    clocks_hw->sleep_en0 = clock0_orig;
    clocks_hw->sleep_en1 = clock1_orig;

    // reset clocks
    clocks_init();
    stdio_init_all();
#endif
}

void Sleep::sleep() {
    // TODO
}
