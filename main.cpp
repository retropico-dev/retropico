//
// Created by cpasjuste on 30/05/23.
//

/**
 * Copyright (C) 2022 by Mahyar Koshkouei <mk@deltabeard.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#ifndef ENABLE_SOUND
#define ENABLE_SOUND 1
#endif
#ifdef ENABLE_RAM_BANK
#define ENABLE_RAM_BANK 1
#endif

/* C Headers */
#include <cstdio>
#include <cstdlib>
#include <cstring>

/* Project headers */
#include "platform.h"
#include "clock.h"
#ifdef LINUX
#include "platform_linux.h"
#else
#include "platform_pico.h"
#endif
#include "gbcolors.h"
#include "hedley.h"
#include "peanut_gb.h"

#ifdef ENABLE_RAM_BANK
static unsigned char rom_bank0[65536];
#endif
static uint8_t *rom = nullptr;
static uint8_t ram[32768];
static int lcd_line_busy = 0;

static palette_t palette;
static uint8_t manual_palette_selected = 0;

static mb::Platform *platform;

/* Multicore command structure. */
union core_cmd {
    struct {
        /* Does nothing. */
#define CORE_CMD_NOP        0
        /* Set line "data" on the LCD. Pixel data is in pixels_buffer. */
#define CORE_CMD_LCD_LINE    1
        /* Control idle mode on the LCD. Limits colours to 2 bits. */
#define CORE_CMD_IDLE_SET    2
        uint8_t cmd;
        uint8_t data;
    };
    uint32_t full;
};

struct gb_priv {
    uint32_t lcd_line_hashes[LCD_HEIGHT];
    uint dma_pixel_buffer_chan;
};
static struct gb_priv gb_priv = {0};

/* Pixel data is stored in here. */
static uint8_t pixels_buffer[LCD_WIDTH];

/**
 * Returns a byte from the ROM file at the given address.
 */
uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
#ifdef ENABLE_RAM_BANK
    if (addr < sizeof(rom_bank0)) {
        return rom_bank0[addr];
    }
#endif

    return rom[addr];
}

/**
 * Returns a byte from the cartridge RAM at the given address.
 */
uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
    return ram[addr];
}

/**
 * Writes a given byte to the cartridge RAM at the given address.
 */
void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
    (void) gb;
    ram[addr] = val;
}

/**
 * Ignore all errors.
 */
void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
    (void) gb;
    (void) val;
#if 1
    const char *gb_err_str[GB_INVALID_MAX] = {
            "UNKNOWN",
            "INVALID OPCODE",
            "INVALID READ",
            "INVALID WRITE",
            "HALT"
    };
    printf("Error %d occurred: %s (abort)\r\n",
           gb_err,
           gb_err >= GB_INVALID_MAX ? gb_err_str[0] : gb_err_str[gb_err]);
    abort();
#endif
}

void core1_lcd_draw_line(const uint_fast8_t line) {
    // write line to display buffer
    for (uint_fast8_t x = 0; x < LCD_WIDTH; x++) {
        uint16_t p = palette[(pixels_buffer[x] & LCD_PALETTE_ALL) >> 4][pixels_buffer[x] & 3];
        platform->getDisplay()->drawPixel({(int16_t) x, (int16_t) line}, p);
    }

    // flip
    if (line == platform->getDisplay()->getSize().y - 1) {
        platform->getDisplay()->flip();
    }

    __atomic_store_n(&lcd_line_busy, 0, __ATOMIC_SEQ_CST);
}

_Noreturn void main_core1() {
    union core_cmd cmd{};

    /* Handle commands coming from core0. */
    while (true) {
        cmd.full = multicore_fifo_pop_blocking();
        switch (cmd.cmd) {
            case CORE_CMD_LCD_LINE:
                core1_lcd_draw_line(cmd.data);
                break;
#if 0
                case CORE_CMD_IDLE_SET:
                    mk_ili9225_display_control(true, cmd.data);
                    break;
#endif
            case CORE_CMD_NOP:
            default:
                break;
        }
    }

    HEDLEY_UNREACHABLE();
}

void lcd_draw_line(struct gb_s *gb, const uint8_t pixels[LCD_WIDTH], const uint_fast8_t line) {
    union core_cmd cmd{};

    /* Wait until previous line is sent. */
    while (__atomic_load_n(&lcd_line_busy, __ATOMIC_SEQ_CST))
        tight_loop_contents();

    memcpy(pixels_buffer, pixels, LCD_WIDTH);

    /* Populate command. */
    cmd.cmd = CORE_CMD_LCD_LINE;
    cmd.data = line;

    __atomic_store_n(&lcd_line_busy, 1, __ATOMIC_SEQ_CST);
    multicore_fifo_push_blocking(cmd.full);

#if LINUX
    // no threading for now...
    //printf("core1_lcd_draw_line\n");
    core1_lcd_draw_line(cmd.data);
#endif
}

int main() {
    static struct gb_s gb;
    enum gb_init_error_e ret;
    uint_fast32_t frames = 0;
#ifndef NDEBUG
    mb::Clock clock;
#endif

#ifdef LINUX
    platform = new mb::LinuxPlatform();
#else
    platform = new mb::PicoPlatform();
#endif

    // load rom from fs
    rom = platform->getIo()->load("/roms/rom.gb");
#ifdef ENABLE_RAM_BANK
    memcpy(rom_bank0, rom, sizeof(rom_bank0));
#endif

    // start Core1, which processes requests to the LCD
    multicore_launch_core1(main_core1);

    // initialise GB context
    ret = gb_init(&gb, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, &gb_priv);

    if (ret != GB_INIT_NO_ERROR) {
        printf("error: %d\r\n", ret);
        stdio_flush();
#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
        while (true) { __wfi(); }
#pragma clang diagnostic pop
        HEDLEY_UNREACHABLE();
    }

    // automatically assign a colour palette to the game
    char rom_title[16];
    auto_assign_palette(palette, gb_colour_hash(&gb), gb_get_rom_name(&gb, rom_title));

    gb_init_lcd(&gb, &lcd_draw_line);
    //gb.direct.interlace = 1;

    while (true) {
        gb.gb_frame = 0;

        do {
            __gb_step_cpu(&gb);
            tight_loop_contents();
        } while (HEDLEY_LIKELY(gb.gb_frame == 0));

        platform->getAudio()->loop();

        /* Required since we do not know whether a button remains
         * pressed over a serial connection. */
        if (frames % 4 == 0) gb.direct.joypad = 0xFF;

        // handle input
        uint16_t buttons = platform->getInput()->getButtons();
        if (buttons > 0 && !(buttons & mb::Input::Button::DELAY)) {
            // exit requested (linux)
            if (buttons & mb::Input::Button::QUIT) break;

            // emulation inputs
            gb.direct.joypad_bits.a = !(buttons & mb::Input::Button::B1);
            gb.direct.joypad_bits.b = !(buttons & mb::Input::Button::B2);
            gb.direct.joypad_bits.select = !(buttons & mb::Input::Button::SELECT);
            gb.direct.joypad_bits.start = !(buttons & mb::Input::Button::START);
            gb.direct.joypad_bits.up = !(buttons & mb::Input::Button::UP);
            gb.direct.joypad_bits.right = !(buttons & mb::Input::Button::RIGHT);
            gb.direct.joypad_bits.down = !(buttons & mb::Input::Button::DOWN);
            gb.direct.joypad_bits.left = !(buttons & mb::Input::Button::LEFT);

            // hotkey / combos
            if (buttons & mb::Input::Button::SELECT) {
                platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
                // palette selection
                if (buttons & mb::Input::Button::LEFT) {
                    if (manual_palette_selected > 0) {
                        manual_palette_selected--;
                        manual_assign_palette(palette, manual_palette_selected);
                    }
                } else if (buttons & mb::Input::Button::RIGHT) {
                    if (manual_palette_selected < NUMBER_OF_MANUAL_PALETTES) {
                        manual_palette_selected++;
                        manual_assign_palette(palette, manual_palette_selected);
                    }
                }
            } else {
                platform->getInput()->setRepeatDelay(0);
            }
        }

#ifndef NDEBUG
        // fps
        if (clock.getElapsedTime().asSeconds() >= 1) {
            printf("fps: %i\r\n", (int) ((float) frames / clock.restart().asSeconds()));
            frames = 0;
        }
#endif
        frames++;
    }

    delete (platform);
    return 0;
}
