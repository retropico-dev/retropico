//
// Created by cpasjuste on 02/06/23.
//

#include "pico.h"

//#define CORE1_EMULATION

#ifdef CORE1_EMULATION
#include <thread>
#include <mutex>

static std::thread core1_thread;
static std::mutex fifo_mutex;
static uint32_t fifo_data = 0;

void multicore_launch_core1(void (*entry)()) {
    core1_thread = std::thread(entry);
}

uint32_t multicore_fifo_pop_blocking() {
    uint32_t data;
    while (true) {
        fifo_mutex.lock();
        if (fifo_data != 0) {
            data = fifo_data;
            fifo_data = 0;
            fifo_mutex.unlock();
            break;
        }
        fifo_mutex.unlock();
    }

    return data;
}

void multicore_fifo_push_blocking(uint32_t data) {
    while (true) {
        fifo_mutex.lock();
        if (fifo_data == 0) {
            fifo_data = data;
            fifo_mutex.unlock();
            break;
        }
        fifo_mutex.unlock();
    }
}
#else

void multicore_launch_core1(void (*entry)()) {}

uint32_t multicore_fifo_pop_blocking() { return 0; }

void multicore_fifo_push_blocking(uint32_t data) {}

#endif
