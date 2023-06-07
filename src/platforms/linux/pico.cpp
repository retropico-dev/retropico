//
// Created by cpasjuste on 02/06/23.
//

#include "pico.h"

void multicore_launch_core1(void (*entry)()) {}

uint32_t multicore_fifo_pop_blocking() { return 0; }

void multicore_fifo_push_blocking(uint32_t data) {}
