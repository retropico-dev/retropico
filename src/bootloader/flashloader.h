//****************************************************************************
// Copyright 2021 Richard Hulme
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Flashloader for the RP2040

#ifndef FLASHLOADER_H
#define FLASHLOADER_H

#include <stdint.h>

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

// 4MB available of 8MB (Feather rp2040) (1MB for rom data, 3MB for misc data)
#define FLASH_TARGET_OFFSET_ROM_HEADER ((1024 * 1024) * 4)  // unused for now
//#define FLASH_TARGET_OFFSET_ROM_HEADER ((uint32_t) &__FLASHLOADER_END)
#define FLASH_TARGET_OFFSET_ROM_DATA (FLASH_TARGET_OFFSET_ROM_HEADER + FLASH_SECTOR_SIZE)
#define FLASH_TARGET_OFFSET_MISC (FLASH_TARGET_OFFSET_ROM_DATA + FLASH_BLOCK_SIZE + (1024 * 1024)) // 1MB max rom size

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
