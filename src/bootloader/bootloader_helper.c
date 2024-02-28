//
// Created by cpasjuste on 28/02/24.
//

#include <stdbool.h>
#include <pico.h>
#include <hardware/dma.h>
#include <hardware/watchdog.h>
#include <hardware/flash.h>
#include <hardware/structs/clocks.h>
#include <hardware/xosc.h>
#include <hardware/pll.h>
#include <hardware/clocks.h>
#include "bootloader.h"
#include "bootloader_helper.h"

#define flashoffset(x) ((uint32_t)(x) - XIP_BASE)

uint32_t sAppStartOffset;

// Nothing else is running on the system, so it doesn't matter which
// DMA channel we use
const uint8_t sDMAChannel = 0;

// Buffer to store a page's worth of data for flashing.
// Must be aligned to a 256 byte boundary to allow use as a DMA ring buffer
uint8_t sPageBuffer[256] __attribute__ ((aligned(256)));

//****************************************************************************
// Calculate the CRC32 (no reflection, no final XOR) of a block of data.
// This makes use of the DMA sniffer to calculate the CRC for us.  Speed is
// not really a huge issue as most of the time we just need to check the
// boot2 image is valid (252 bytes) but using DMA ought to be faster than
// looping over the data without a lookup table and is certainly a lot smaller
// than the lookup table.
uint32_t crc32(const void *data, size_t len, uint32_t crc) {
    uint8_t dummy;

    dma_channel_config c = dma_channel_get_default_config(sDMAChannel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    channel_config_set_sniff_enable(&c, true);

    // Turn on CRC32 (non-bit-reversed data)
    dma_sniffer_enable(sDMAChannel, 0x00, true);
    dma_hw->sniff_data = crc;

    dma_channel_configure(
            sDMAChannel,
            &c,
            &dummy,
            data,
            len,
            true    // Start immediately
    );

    dma_channel_wait_for_finish_blocking(sDMAChannel);

    return (dma_hw->sniff_data);
}


//****************************************************************************
// Prepare DMA channel to read a page (256 bytes) worth of data into the
// page buffer starting from the given address.
// The copying will be triggered by calling 'copyPage'
void copyPageInit(const void *src) {
    dma_channel_config c = dma_channel_get_default_config(sDMAChannel);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, true);
    channel_config_set_sniff_enable(&c, true);
    channel_config_set_ring(&c, true, 8); // (log2(256) == 8)

    // Turn on CRC32 (non-bit-reversed data)
    dma_sniffer_enable(sDMAChannel, 0x00, true);
    dma_hw->sniff_data = 0xffffffff;

    dma_channel_configure(
            sDMAChannel,
            &c,
            sPageBuffer,
            src,
            256 / 4,
            false    // Do not start immediately
    );
}

//****************************************************************************
// Copy the next page of data to the page buffer and return the updated CRC32
uint32_t copyPage() {
    dma_channel_start(sDMAChannel);
    dma_channel_wait_for_finish_blocking(sDMAChannel);
    return (dma_hw->sniff_data);
}


//****************************************************************************
// Flash the main application using the provided image.
void flashFirmware(const tFlashHeader *header, uint32_t eraseLength) {
    uint32_t crc = 0;
    uint32_t offset = 256;

    // Start the watchdog and give us 500ms for each erase/write cycle.
    // This should be more than enough time but in case anything happens,
    // we'll reset and try again.
    watchdog_reboot(0, 0, 500);

    // Erase the target memory area
    uint32_t start = flashoffset(sAppStartOffset);

    for (uint32_t sectors = eraseLength / FLASH_SECTOR_SIZE; sectors > 0; sectors--) {
        flash_range_erase(start, FLASH_SECTOR_SIZE);
        start += FLASH_SECTOR_SIZE;
        watchdog_update();
    }

    // Write everything except the first page.  If there's any kind
    // of power failure during writing, this will prevent anything
    // trying to boot the partially flashed image

    // Get total number of pages - 1 (because we're flashing the first page
    // separately)
    uint32_t pages = ((header->length - 1) >> 8);

    // Prepare the DMA channel for copying
    copyPageInit(header->data + offset);

    while (pages > 0) {
        // Reset the watchdog counter
        watchdog_update();

        // Copy the page to a RAM buffer so we're not trying to read from
        // flash whilst writing to it
        crc = copyPage();
        flash_range_program(flashoffset(sAppStartOffset + offset),
                            sPageBuffer,
                            256);
        offset += 256;
        pages--;
    }

    // Reset the watchdog counter
    watchdog_update();

    // Check that everything so far has been written correctly
    if (crc == crc32(&header->data[256], offset - 256, 0xffffffff)) {
        // Now flash the first page which is the boot2 image with CRC.
        copyPageInit(header->data);
        crc = copyPage();

        flash_range_program(flashoffset(sAppStartOffset),
                            sPageBuffer,
                            256);

        // Reset the watchdog counter
        watchdog_update();

        if (crc == crc32(header->data, 256, 0xffffffff)) {
            // Invalidate the start of the flash image to prevent it being
            // picked up again (prevents cyclic flashing if the image is bad)
            flash_range_erase(flashoffset(header), 4096);

            // Indicate to the application that it has been updated
            watchdog_hw->scratch[0] = FLASH_APP_UPDATED;
        }
    }

    // Disable the watchdog
    hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
}

//****************************************************************************
// Configure one of either clk_ref or clk_sys.
//
// This is mostly lifted from clock_configure with some code removed that
// doesn't apply to clk_ref or clk_sys and using a fixed divisor
void configClock(enum clock_index clk_index, uint32_t src, uint32_t auxsrc) {
    clock_hw_t *clock = &clocks_hw->clk[clk_index];

    clock->div = 0x100; // divisor == 1.00

    if (src == CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX) {
        hw_clear_bits(&clock->ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
        while (!(clock->selected & 1u))
            tight_loop_contents();
    }

    // Set aux mux first, and then glitchless mux
    hw_write_masked(&clock->ctrl,
                    (auxsrc << CLOCKS_CLK_SYS_CTRL_AUXSRC_LSB),
                    CLOCKS_CLK_SYS_CTRL_AUXSRC_BITS);

    hw_write_masked(&clock->ctrl,
                    src << CLOCKS_CLK_REF_CTRL_SRC_LSB,
                    CLOCKS_CLK_REF_CTRL_SRC_BITS);
    while (!(clock->selected & (1u << src)))
        tight_loop_contents();
}

//****************************************************************************
// Use the external oscillator as the clock reference to gain a bit of speed!
void initClock() {
    // Start tick in watchdog
    watchdog_start_tick(XOSC_MHZ);

    clocks_hw->resus.ctrl = 0;

    xosc_init();

    // Before we touch PLLs, switch sys and ref cleanly away from their aux sources.
    hw_clear_bits(&clocks_hw->clk[clk_sys].ctrl, CLOCKS_CLK_SYS_CTRL_SRC_BITS);
    while (clocks_hw->clk[clk_sys].selected != 0x1)
        tight_loop_contents();
    hw_clear_bits(&clocks_hw->clk[clk_ref].ctrl, CLOCKS_CLK_REF_CTRL_SRC_BITS);
    while (clocks_hw->clk[clk_ref].selected != 0x1)
        tight_loop_contents();

    pll_init(pll_sys, 1, 1500 * MHZ, 6, 2);

    configClock(clk_ref,
                CLOCKS_CLK_REF_CTRL_SRC_VALUE_XOSC_CLKSRC,
                0); // No aux mux

    configClock(clk_sys,
                CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_SYS);
}

#ifndef USE_PICO_STDLIB

//****************************************************************************
// These functions are normally provided as part of pico_stdlib so we have to
// provide them here if not we're not using it.
void exit(int ret) {
    (void) ret;
    while (true)
        tight_loop_contents();
}

void panic(const char *fmt, ...) {
    (void) fmt;
    while (true)
        tight_loop_contents();
}

void hard_assertion_failure(void) {
    while (true)
        tight_loop_contents();
}

//****************************************************************************
// Provide our own assert function to prevent the default version pulling
// in 'printf' functions in debug builds
void __assert_func(const char *filename,
                   int line,
                   const char *assert_func,
                   const char *expr) {
    (void) filename;
    (void) line;
    (void) assert_func;
    (void) expr;

    __breakpoint();

    while (true)
        tight_loop_contents();
}

#endif
