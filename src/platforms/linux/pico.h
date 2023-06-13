//
// Created by cpasjuste on 07/06/23.
//

#ifndef MICROBOY_PICO_H
#define MICROBOY_PICO_H

#include <cstdint>
#include <unistd.h>

#define _Noreturn [[noreturn]]
#define __not_in_flash_func(func_name) func_name
#define tight_loop_contents() ((void)0)
#define stdio_flush() fflush(stdout)
#define reset_usb_boot(x, y) ((void)0)
#define __wfi() usleep(1000)

void multicore_launch_core1(void (*entry)());

uint32_t multicore_fifo_pop_blocking();

void multicore_fifo_push_blocking(uint32_t data);

#endif //MICROBOY_PICO_H
