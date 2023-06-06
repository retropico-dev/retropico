//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_COMPAT_LINUX_H
#define MICROBOY_COMPAT_LINUX_H

void multicore_launch_core1(void (*entry)());

uint32_t multicore_fifo_pop_blocking();

void multicore_fifo_push_blocking(uint32_t data);

#endif //MICROBOY_COMPAT_LINUX_H
