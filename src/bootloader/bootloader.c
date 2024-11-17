//****************************************************************************
// Copyright 2021 Richard Hulme
//
// SPDX-License-Identifier: BSD-3-Clause
//
// Flashloader for the RP2040
//
// If there is no new application to be flashed (according to the watchdog
// scratch registers), the flashloader will simply transfer control to the
// existing application starting in the erase block following the flashloader.
// This is the normal case.
//
// If the watchdog scratch registers are correctly set, the flashloader will
// replace the existing application with the new one stored in the location
// provided and then reboot the processor.
//
// If the existing application is invalid, the flashloader will check
// each erase block for a valid update image and flash that if successful.
//
// If all else fails (application is invalid and no update image can be found)
// the flashloader will drop back to bootrom bootloader.

#include <string.h>
#include <stdio.h>
#include <pico/stdio.h>
#include <pico/time.h>
#include <hardware/gpio.h>
#include "hardware/regs/addressmap.h"
#ifdef PICO_RP2350
#include "hardware/regs/m33.h"
#else
#include "hardware/regs/m0plus.h"
#endif
#include "hardware/structs/watchdog.h"
#include "hardware/resets.h"
#include "hardware/flash.h"
#include "hardware/watchdog.h"
#include "pico/bootrom.h"
#include "pico/binary_info.h"
#include "bootloader.h"

#include "bootloader_helper.h"

#include "../../external/misc/libpico2d/src/platforms/pico/misc/pinout.h"

bi_decl(bi_program_version_string("1.10"));

#if !PICO_FLASH_SIZE_BYTES
#error PICO_FLASH_SIZE_BYTES not defined!
#endif

//****************************************************************************
// We don't normally want to link against pico_stdlib as that pulls in lots of
// other stuff we don't need here and we end up at just under 8k before we've
// even started.
// Sometimes though it *is* helpful to use pico_stdlib for checking weird
// errors aren't because of some missing initialisation here so uncomment this
// define to make the necessary changes here but don't forget to change the
// start address for the application in the linker script!
//#define USE_PICO_STDLIB

#define bl2crc(x)      (*((uint32_t*)(((uint32_t)(x) + 0xfc))))

// The maximum number of times the flashloader will try to flash an image
// before it gives up and boots in the bootrom bootloader
static const uint32_t sMaxRetries = 3;

//****************************************************************************
// Start the main application if its boot2 image is valid.
// Will not return unless the image is invalid
int startMainApplication() {
    if (crc32((const void *) sAppStartOffset, 252, 0xffffffff) == bl2crc(sAppStartOffset)) {
        // Main application appears to be OK so we can map the application's
        // vector table and jump to the start of its code

        // First make sure we don't get retriggered
        if (watchdog_hw->scratch[0] == FLASH_MAGIC1 || watchdog_hw->scratch[0] == ~FLASH_MAGIC1
            || watchdog_hw->scratch[0] == FLASH_MAGIC_UI || watchdog_hw->scratch[0] == ~FLASH_MAGIC_UI
            || watchdog_hw->scratch[0] == FLASH_MAGIC_NES || watchdog_hw->scratch[0] == ~FLASH_MAGIC_NES
            || watchdog_hw->scratch[0] == FLASH_MAGIC_GB || watchdog_hw->scratch[0] == ~FLASH_MAGIC_GB
            || watchdog_hw->scratch[0] == FLASH_MAGIC_SMS || watchdog_hw->scratch[0] == ~FLASH_MAGIC_SMS) {
            watchdog_hw->scratch[0] = 0;
        }

        // Hold DMA block in reset again (in case the application doesn't
        // need DMA and doesn't want to waste power)
        reset_block(RESETS_RESET_DMA_BITS);

        asm volatile (
            "mov r0, %[start]\n"
            "ldr r1, =%[vtable]\n"
            "str r0, [r1]\n"
            "ldmia r0, {r0, r1}\n"
            "msr msp, r0\n"
            "bx r1\n"
            :
#ifdef PICO_RP2350
            : [start] "r"(sAppStartOffset + 0x100), [vtable] "X"(PPB_BASE + M33_VTOR_OFFSET)
#else
            : [start] "r"(sAppStartOffset + 0x100), [vtable] "X"(PPB_BASE + M0PLUS_VTOR_OFFSET)
#endif
            :
        );
    }

    // We will only return if the main application couldn't be started
    return 0;
}

// check for combo keys
bool check_bootloader_combo() {
    printf("BL: check_bootloader_combo\r\n");

    if (BTN_PIN_VOL_D > -1) {
        gpio_set_function(BTN_PIN_VOL_D, GPIO_FUNC_SIO);
        gpio_set_dir(BTN_PIN_VOL_D, GPIO_IN);
        gpio_pull_up(BTN_PIN_VOL_D);
        sleep_ms(10);
        if (!gpio_get(BTN_PIN_VOL_D)) {
            printf("BL: vol down button pressed (reboot bootloader)\r\n");
            reset_usb_boot(0, 0);
            while (true) tight_loop_contents();
        }
    }

    if (BTN_PIN_VOL_U > -1) {
        gpio_set_function(BTN_PIN_VOL_U, GPIO_FUNC_SIO);
        gpio_set_dir(BTN_PIN_VOL_U, GPIO_IN);
        gpio_pull_up(BTN_PIN_VOL_U);
        sleep_ms(10);
        if (!gpio_get(BTN_PIN_VOL_U)) {
            printf("BL: vol up button pressed (ui)\r\n");
            return true;
        }
    }

    return false;
}

// check "config" set by filer to determine which core to load
uint32_t get_core_offset() {
    uint8_t *data = (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET_CONFIG);
    if (memcmp(data, "NES", 3) == 0) {
        return XIP_BASE + (uint32_t) &__NES_START;
    } else if (memcmp(data, "GB", 2) == 0) {
        return XIP_BASE + (uint32_t) &__GB_START;
    } else if (memcmp(data, "SMS", 2) == 0) {
        return XIP_BASE + (uint32_t) &__SMS_START;
    }

    return XIP_BASE + (uint32_t) &__UI_START;
}

#if 0
int isRomInFlash() {
    // check for a valid nes rom
    uint8_t *data = (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET_ROM_DATA);
    if (memcmp(data, "NES\x1a", 4) == 0) {
        return 1;
    }

    // check for a valid gb rom (checksum)
    uint8_t x = 0;
    uint16_t i;
    for (i = 0x0134; i <= 0x014C; i++) {
        x = x - data[i] - 1;
    }
    if (x == data[0x014D]) {
        return 2;
    }

    // check for a valid sms rom
    // https://www.smspower.org/Development/ROMHeader
    if (memcmp(data + 0x7FF0, "TMR SEGA", 8) == 0
        || memcmp(data + 0x3FF0, "TMR SEGA", 8) == 0
        || memcmp(data + 0x1FF0, "TMR SEGA", 8) == 0) {
        return 3;
    }

    // no valid rom found, load ui
    return 0;
}
#endif

//****************************************************************************
int main(void) {
    const tFlashHeader *header;
    uint32_t eraseLength = 0;

#if defined(PICO_DEBUG_UART) || defined(PICO_DEBUG_USB)
    stdio_init_all();
#endif
    printf("BL: starting...\r\n");

    uint32_t scratch = watchdog_hw->scratch[0];
    uint32_t image = watchdog_hw->scratch[1];

    stdio_init_all();
    printf("BL: scratch: 0x%08lx\r\n", scratch);

    // Use xosc, which will give us a speed boost
    initClock();

    // Take DMA block out of reset so we can use it to calculate CRCs
    unreset_block_wait(RESETS_RESET_DMA_BITS);

    // check for combos keys
    if (check_bootloader_combo()) scratch = FLASH_MAGIC_UI;

    // first check for UI magic to not enter a boot loop if rom doesn't work
    if (scratch == FLASH_MAGIC_UI) {
        sAppStartOffset = XIP_BASE + (uint32_t) &__UI_START;
    } else {
        sAppStartOffset = get_core_offset();
    }

    printf("BL: sAppStartOffset: 0x%08lx\r\n", sAppStartOffset);

    if ((scratch == FLASH_MAGIC1) && ((image & 0xfff) == 0) && (image > sAppStartOffset)) {
        // Invert the magic number (so we know we've been here) and
        // initialise the retry counter
        watchdog_hw->scratch[0] = ~FLASH_MAGIC1;
        watchdog_hw->scratch[2] = 0;
    } else if (scratch == ~FLASH_MAGIC1) {
        watchdog_hw->scratch[2]++;
    } else if (!startMainApplication()) {
        printf("BL: startMainApplication failed...\r\n");
        // Tried and failed to start the main application so try to find
        // an update image
        image = sAppStartOffset + 0x1000;

        // In case there are any problems during flashing, make it look like
        // an update was requested so that the retry mechanism is correctly
        // initialised.
        watchdog_hw->scratch[0] = ~FLASH_MAGIC1;
        watchdog_hw->scratch[1] = image;
        watchdog_hw->scratch[2] = 0;
    }

    while (image < (XIP_BASE + PICO_FLASH_SIZE_BYTES)) {
        header = (const tFlashHeader *) image;

        if ((header->magic1 == FLASH_MAGIC1) &&
            (header->magic2 == FLASH_MAGIC2) &&
            (crc32(header->data, header->length, 0xffffffff) == header->crc32) &&
            (crc32(header->data, 252, 0xffffffff) == bl2crc(header->data))) {
            // Round up erase length to next 4k boundary
            eraseLength = (header->length + 4095) & 0xfffff000;

            // Looks like we've found a valid image but only use it if we
            // are sure that it won't get clobbered when we erase the flash
            // before programming.
            if ((sAppStartOffset + eraseLength) < (uint32_t) header)
                break;
            else
                eraseLength = 0;
        }

        image += 0x1000;
    }

    // If we've found a new, valid image, go ahead and flash it!
    if ((eraseLength != 0) && (watchdog_hw->scratch[2] < sMaxRetries)) {
        flashFirmware(header, eraseLength);

        // Reboot into the new image
        watchdog_reboot(0, 0, 50);

        while (true)
            tight_loop_contents();
    }

    // Something's gone wrong.  The main application is corrupt and/or
    // there was a problem with the update image (or it couldn't be found).

    // If we were originally explicitly triggered by a watchdog reset, try
    // to start the normal application since we didn't before
    if ((scratch == FLASH_MAGIC1) || (scratch == ~FLASH_MAGIC1))
        startMainApplication();

    printf("BL: warning: rebooting to bootloader... \r\n");

    // Otherwise go to the bootrom bootloader as a last resort
    reset_usb_boot(0, 0);
}
