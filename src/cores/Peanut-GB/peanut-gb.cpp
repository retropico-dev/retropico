//
// Created by cpasjuste on 06/06/23.
//

extern "C" {
#define ENABLE_SOUND 1
#include "minigb_apu.h"
#include "peanut_gb.h"
#include "gbcolors.h"
#include "hedley.h"
};

#include "peanut-gb.h"

using namespace mb;
using namespace p2d;

#undef inline
#define inline __always_inline

#define ENABLE_RAM_BANK
#ifdef ENABLE_RAM_BANK
static uint8_t rom_bank0[65536];
#endif
static const uint8_t *gb_rom = nullptr;
static uint8_t gb_ram[32768];

static palette_t palette;
static uint8_t manual_palette_selected = 0;

#if ENABLE_SOUND
#define AUDIO_BUFFER_SIZE (AUDIO_SAMPLES * 4)
static uint16_t audio_stream[AUDIO_BUFFER_SIZE];
#endif

static Display *s_display;

static struct gb_s gameboy;

uint8_t gb_rom_read(struct gb_s *gb, uint_fast32_t addr);

uint8_t gb_cart_ram_read(struct gb_s *gb, uint_fast32_t addr);

void gb_cart_ram_write(struct gb_s *gb, uint_fast32_t addr, uint8_t val);

void gb_error(struct gb_s *gb, enum gb_error_e gb_err, uint16_t val);

static inline void in_ram(lcd_draw_line)(struct gb_s *gb, const uint8_t pixels[LCD_WIDTH], uint_fast8_t line);

PeanutGB::PeanutGB(Platform *p) : Core(p, Core::Type::Gb) {
    s_display = p_platform->getDisplay();

#if ENABLE_SOUND
    // init audio
    p_platform->getAudio()->setup(AUDIO_SAMPLE_RATE, AUDIO_SAMPLES);
    audio_init();
#endif
}

bool PeanutGB::loadRom(Io::FileBuffer file) {
    enum gb_init_error_e ret;

    printf("PeanutGB::loadRom: %s\r\n", file.name);
    m_romName = file.name;
    m_sramPath = Core::getSavePath(Core::Type::Sms) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";

    gb_rom = file.data;
#ifdef ENABLE_RAM_BANK
    memcpy(rom_bank0, file.data, sizeof(rom_bank0));
#endif

    // initialise GB context
    ret = gb_init(&gameboy, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, nullptr);

    if (ret != GB_INIT_NO_ERROR) {
        printf("PeanutGB::loadRom: error %d\r\n", ret);
        return false;
    }

    // automatically assign a colour palette to the game
    char rom_title[16];
    auto_assign_palette(palette, gb_colour_hash(&gameboy), gb_get_rom_name(&gameboy, rom_title));

    gb_init_lcd(&gameboy, &lcd_draw_line);

    // not enough fps when scaling is used (with double buffering, st7789), enable interlacing
    //if (m_scaling) gameboy.direct.interlace = 1;

    return true;
}

bool in_ram(PeanutGB::loop)(uint16_t buttons) {
    if (!Core::loop(buttons)) return false;

    gameboy.gb_frame = 0;
    do {
        __gb_step_cpu(&gameboy);
        tight_loop_contents();
    } while (HEDLEY_LIKELY(gameboy.gb_frame == 0));

#if ENABLE_SOUND
    // send audio buffer to playback device
    audio_callback(nullptr, reinterpret_cast<uint8_t *>(audio_stream), AUDIO_BUFFER_SIZE);
    p_platform->getAudio()->play(audio_stream, AUDIO_SAMPLES);
#endif

    /* Required since we do not know whether a button remains
     * pressed over a serial connection. */
    //if (frames % 4 == 0) gameboy.direct.joypad = 0xFF;
    gameboy.direct.joypad = 0xFF;

    // handle input
    // emulation inputs
    gameboy.direct.joypad_bits.a = !(buttons & p2d::Input::Button::B1);
    gameboy.direct.joypad_bits.b = !(buttons & p2d::Input::Button::B2);
    gameboy.direct.joypad_bits.select = !(buttons & p2d::Input::Button::SELECT);
    gameboy.direct.joypad_bits.start = !(buttons & p2d::Input::Button::START);
    gameboy.direct.joypad_bits.up = !(buttons & p2d::Input::Button::UP);
    gameboy.direct.joypad_bits.right = !(buttons & p2d::Input::Button::RIGHT);
    gameboy.direct.joypad_bits.down = !(buttons & p2d::Input::Button::DOWN);
    gameboy.direct.joypad_bits.left = !(buttons & p2d::Input::Button::LEFT);

#warning "TODO: hotkeys / combos"
    /*
    // hotkeys / combos
    if (buttons & p2d::Input::Button::SELECT) {
        p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
        // palette selection
        if (buttons & p2d::Input::Button::LEFT) {
            if (manual_palette_selected > 0) {
                manual_palette_selected--;
                manual_assign_palette(palette, manual_palette_selected);
            }
        } else if (buttons & p2d::Input::Button::RIGHT) {
            if (manual_palette_selected < NUMBER_OF_MANUAL_PALETTES) {
                manual_palette_selected++;
                manual_assign_palette(palette, manual_palette_selected);
            }
        } else if (buttons & p2d::Input::Button::UP) {
            setScalingEnabled(true);
        } else if (buttons & p2d::Input::Button::DOWN) {
            setScalingEnabled(false);
        }
    } else {
        p_platform->getInput()->setRepeatDelay(0);
    }
    */

    return true;
}

inline uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
#ifdef ENABLE_RAM_BANK
    if (addr < sizeof(rom_bank0)) {
        return rom_bank0[addr];
    }
#endif

    return gb_rom[addr];
}

inline uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
    return gb_ram[addr];
}

inline void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
    (void) gb;
    gb_ram[addr] = val;
}

inline void gb_error(struct gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
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

static inline void in_ram(lcd_draw_line)(struct gb_s *gb, const uint8_t pixels[LCD_WIDTH], const uint_fast8_t line) {
    uint8_t *buffer = s_display->getFramebuffer()->getPixels();
    uint8_t bpp = s_display->getFramebuffer()->getBpp();

    for (uint_fast8_t x = 0; x < LCD_WIDTH; x++) {
        // do not use "setPixel", use display framebuffer directly for speedup
        *(uint16_t *) (buffer + line * (LCD_WIDTH * bpp) + x * bpp)
                = palette[(pixels[x] & LCD_PALETTE_ALL) >> 4][pixels[x] & 3];
    }

    if (line == LCD_HEIGHT - 1) {
        s_display->flip();
    }
}
