//
// Created by cpasjuste on 30/05/23.
//

#include <thread>
#include "platform_compat.h"

void multicore_launch_core1(void (*entry)()) {
    //std::thread t1(entry);
}

uint32_t multicore_fifo_pop_blocking() {
    return 0;
}

void multicore_fifo_push_blocking(uint32_t data) {

}