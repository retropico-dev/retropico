//****************************************************************************
// Copyright 2021 Richard Hulme
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Flashloader for the RP2040

#ifndef FLASHLOADER_H
#define FLASHLOADER_H

#include <stdint.h>

// put bootloader "settings" just before libpico2d "FLASH_TARGET_OFFSET_CACHE"
//#include "../../external/misc/libpico2d/src/platforms/pico/storage/flash.h"
#define FLASH_TARGET_OFFSET_CACHE ((1024 * 1024) * 5)   // 1MB flash cache
#define FLASH_TARGET_OFFSET_CONFIG (FLASH_TARGET_OFFSET_CACHE - FLASH_SECTOR_SIZE)

static const uint32_t FLASH_MAGIC1 = 0x8ecd5efb; // Randomly picked numbers
static const uint32_t FLASH_MAGIC2 = 0xc5ae52a0;
static const uint32_t FLASH_MAGIC_UI = 0xc5ae52a1;
static const uint32_t FLASH_MAGIC_NES = 0xc5ae52a2;
static const uint32_t FLASH_MAGIC_GB = 0xc5ae52a3;
static const uint32_t FLASH_MAGIC_SMS = 0xc5ae52a4;

static const uint32_t FLASH_APP_UPDATED = 0xe3fa4ef2; // App has been updated

extern void *__UI_START;
extern void *__NES_START;
extern void *__GB_START;
extern void *__SMS_START;
extern void *__FLASHLOADER_END;

#ifndef __LINUX__
typedef struct __packed __aligned(4) {
    uint32_t magic1;
    uint32_t magic2;
    uint32_t length;
    uint32_t crc32;
    uint8_t data[];
} tFlashHeader;
#endif
#endif // FLASHLOADER_H
