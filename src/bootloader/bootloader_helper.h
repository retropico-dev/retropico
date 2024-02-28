//
// Created by cpasjuste on 28/02/24.
//

#ifndef RETROPICO_BOOTLOADER_HELPER_H
#define RETROPICO_BOOTLOADER_HELPER_H

extern uint8_t sPageBuffer[256];

extern uint32_t sAppStartOffset; // = XIP_BASE + (uint32_t) &__NES_START;

uint32_t crc32(const void *data, size_t len, uint32_t crc);

void copyPageInit(const void *src);

uint32_t copyPage();

void configClock(enum clock_index clk_index, uint32_t src, uint32_t auxsrc);

void initClock();

void flashFirmware(const tFlashHeader *header, uint32_t eraseLength);

#endif //RETROPICO_BOOTLOADER_HELPER_H
