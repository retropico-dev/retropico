//
// Created by cpasjuste on 01/06/23.
//

#include <cstring>
#include "platform.h"

using namespace mb;

#define FLASH_TARGET_OFFSET (1024 * 1024)
extern "C" const unsigned char flash_rom[];
extern "C" unsigned int flash_rom_len;

uint8_t *PicoIo::load(const std::string &romPath, size_t *size) {
    uint8_t buffer[FLASH_SECTOR_SIZE];
    uint32_t ints = save_and_disable_interrupts();

    printf("PicoIo::load: %s, writing file to flash ", romPath.c_str());
    for (uint32_t i = 0; i < flash_rom_len; i += FLASH_SECTOR_SIZE) {
        printf(".");
        stdio_flush();
        // we can't read and write from flash as the same time
        memcpy(buffer, flash_rom + i, FLASH_SECTOR_SIZE);
        flash_range_erase(FLASH_TARGET_OFFSET + i, FLASH_SECTOR_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET + i, buffer, FLASH_SECTOR_SIZE);
    }

    restore_interrupts(ints);

    printf(" done\r\n");

    if (*size) *size = flash_rom_len;

    return (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
}
