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

using namespace retropico;
using namespace p2d;

#undef inline
#define inline __always_inline

#if PICO_RP2350
#define ROM_BANK_SIZE_MAX (65536 * 4)
static uint8_t rom_bank0[ROM_BANK_SIZE_MAX];
#define RAM_BANK_SIZE_MAX (32768 * 2)
static uint8_t gb_ram[RAM_BANK_SIZE_MAX];
#else
#define ROM_BANK_SIZE_MAX (65536)
static uint8_t rom_bank0[ROM_BANK_SIZE_MAX];
#define RAM_BANK_SIZE_MAX (32768)
static uint8_t gb_ram[RAM_BANK_SIZE_MAX];
#endif

static const uint8_t *gb_rom = nullptr;

// boot rom
extern const unsigned char dmg_boot_bin[];
extern unsigned int dmg_boot_bin_len;

static palette_t palette;
static uint8_t manual_palette_selected = 0;

#if ENABLE_SOUND
#define AUDIO_BUFFER_SIZE (AUDIO_SAMPLES * 4)
static uint16_t audio_stream[AUDIO_BUFFER_SIZE];
#endif

static Display *s_display;
static gb_s s_gameboy;

uint8_t gb_bootrom_read(gb_s *gb, uint_fast16_t addr);

uint8_t gb_rom_read(gb_s *gb, uint_fast32_t addr);

uint8_t gb_cart_ram_read(gb_s *gb, uint_fast32_t addr);

void gb_cart_ram_write(gb_s *gb, uint_fast32_t addr, uint8_t val);

bool gb_cart_ram_load(const std::string &path, size_t len);

bool gb_cart_ram_save(const std::string &path, size_t len);

void gb_error(gb_s *gb, gb_error_e gb_err, uint16_t val);

static inline void in_ram(lcd_draw_line)(gb_s *gb, const uint8_t pixels[LCD_WIDTH], uint_fast8_t line);

PeanutGB::PeanutGB(const Display::Settings &ds) : Core(ds, Gb) {
    s_display = getDisplay();

#if ENABLE_SOUND
    // init audio
    getAudio()->setup(AUDIO_SAMPLE_RATE, AUDIO_SAMPLES);
    audio_init();
#endif
}

bool PeanutGB::loadRom(const Io::File &file) {
    printf("InfoNES::loadRom: %s (%lu bytes)\r\n", file.getPath().c_str(), file.getLength());
    m_romName = file.getName();
    m_sramPath = getSavesPath(Gb) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";

    gb_rom = file.getPtr();
    const size_t size = file.getLength() > ROM_BANK_SIZE_MAX ? ROM_BANK_SIZE_MAX : file.getLength();
    memcpy(rom_bank0, file.getPtr(), size);

    // initialise GB context
    const auto ret = gb_init(&s_gameboy, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, nullptr);
    if (ret != GB_INIT_NO_ERROR) {
        printf("PeanutGB::loadRom: error %d\r\n", ret);
        return false;
    }

    // load bootrom
    gb_set_bootrom(&s_gameboy, gb_bootrom_read);
    gb_reset(&s_gameboy);

    // load sram if any
    gb_cart_ram_load(m_sramPath, gb_get_save_size(&s_gameboy));

    // automatically assign a colour palette to the game
    char rom_title[16];
    auto_assign_palette(palette, gb_colour_hash(&s_gameboy), gb_get_rom_name(&s_gameboy, rom_title));
    gb_init_lcd(&s_gameboy, &lcd_draw_line);

    return true;
}

bool in_ram(PeanutGB::loop)() {
    if (!Core::loop()) return false;

    s_gameboy.gb_frame = false;
    do {
        __gb_step_cpu(&s_gameboy);
        tight_loop_contents();
    } while (HEDLEY_LIKELY(s_gameboy.gb_frame == 0));

#if ENABLE_SOUND
    // send audio buffer to playback device
    audio_callback(nullptr, reinterpret_cast<uint8_t *>(audio_stream), AUDIO_BUFFER_SIZE);
    getAudio()->play(audio_stream, AUDIO_SAMPLES);
#endif

    /* Required since we do not know whether a button remains
     * pressed over a serial connection. */
    //if (frames % 4 == 0) gameboy.direct.joypad = 0xFF;
    s_gameboy.direct.joypad = 0xFF;

    // handle input
    // emulation inputs
    const auto buttons = getInput()->getButtons();
    s_gameboy.direct.joypad_bits.a = !(buttons & Input::Button::B1);
    s_gameboy.direct.joypad_bits.b = !(buttons & Input::Button::B2);
    s_gameboy.direct.joypad_bits.select = !(buttons & Input::Button::SELECT);
    s_gameboy.direct.joypad_bits.start = !(buttons & Input::Button::START);
    s_gameboy.direct.joypad_bits.up = !(buttons & Input::Button::UP);
    s_gameboy.direct.joypad_bits.right = !(buttons & Input::Button::RIGHT);
    s_gameboy.direct.joypad_bits.down = !(buttons & Input::Button::DOWN);
    s_gameboy.direct.joypad_bits.left = !(buttons & Input::Button::LEFT);

    // hotkeys / combos
    if (buttons & Input::Button::SELECT) {
        getInput()->setRepeatDelay(INPUT_DELAY_UI);
        if (!(buttons & Input::Button::DELAY)) {
            // palette selection
            if (buttons & Input::Button::LEFT) {
                if (manual_palette_selected > 0) {
                    manual_palette_selected--;
                    manual_assign_palette(palette, manual_palette_selected);
                }
            } else if (buttons & Input::Button::RIGHT) {
                if (manual_palette_selected < NUMBER_OF_MANUAL_PALETTES - 1) {
                    manual_palette_selected++;
                    manual_assign_palette(palette, manual_palette_selected);
                }
            }
        }
    } else if (!(buttons & Input::Button::DELAY)) {
        getInput()->setRepeatDelay(0);
    }

    return true;
}

void PeanutGB::close() {
    printf("PeanutGB::close()\r\n");
    // save sram
    if (!gb_cart_ram_save(m_sramPath, gb_get_save_size(&s_gameboy))) {
        printf("PeanutGB::close(): sram save failed...\r\n");
    }
}

static inline void in_ram(lcd_draw_line)(gb_s *gb, const uint8_t pixels[LCD_WIDTH], const uint_fast8_t line) {
    const auto buffer = s_display->getFramebuffer()->getPixels();
    const auto bpp = s_display->getFramebuffer()->getBpp();

    for (uint_fast8_t x = 0; x < LCD_WIDTH; x++) {
        // do not use "setPixel", use display framebuffer directly for speedup
        *reinterpret_cast<uint16_t *>(buffer + line * (LCD_WIDTH * bpp) + x * bpp)
                = palette[(pixels[x] & LCD_PALETTE_ALL) >> 4][pixels[x] & 3];
    }

    if (line == LCD_HEIGHT - 1) {
        s_display->flip();
    }
}

inline uint8_t gb_bootrom_read(gb_s *gb, const uint_fast16_t addr) {
    return dmg_boot_bin[addr];
}

inline uint8_t gb_rom_read(gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
    if (addr < ROM_BANK_SIZE_MAX) {
        return rom_bank0[addr];
    }

    //printf("gb_rom_read: warn: addr > ROM_BANK0_SIZE\r\n");
    return gb_rom[addr];
}

inline uint8_t gb_cart_ram_read(gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
    return gb_ram[addr];
}

inline void gb_cart_ram_write(gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
    (void) gb;
    gb_ram[addr] = val;
}

inline bool gb_cart_ram_load(const std::string &path, const size_t len) {
    printf("gb_cart_ram_load: loading sram from %s\r\n", path.c_str());

    if (len > RAM_BANK_SIZE_MAX) {
        printf("gb_cart_ram_load: cart ram is bigger than RAM_BANK_SIZE_MAX\r\n");
        return false;
    }

    const Io::File file{path, Io::File::OpenMode::Read};
    if (!file.isOpen()) {
        printf("gb_cart_ram_load: could not open file for reading...\r\n");
        return false;
    }

    const int read = file.read(0, RAM_BANK_SIZE_MAX, reinterpret_cast<char *>(gb_ram));
    return read == RAM_BANK_SIZE_MAX;
}

inline bool gb_cart_ram_save(const std::string &path, const size_t len) {
    printf("gb_cart_ram_save: saving sram (%i) to %s\r\n", len, path.c_str());

    if (len > RAM_BANK_SIZE_MAX) {
        printf("gb_cart_ram_save: cart ram is bigger than RAM_BANK_SIZE_MAX\r\n");
        return false;
    }

    const Io::File file{path, Io::File::OpenMode::Write};
    if (!file.isOpen()) {
        printf("gb_cart_ram_save: could not open file for writing...\r\n");
        return false;
    }

    const int wrote = file.write(0, RAM_BANK_SIZE_MAX, reinterpret_cast<const char *>(gb_ram));
    return wrote == RAM_BANK_SIZE_MAX;
}

inline void gb_error(gb_s *gb, const enum gb_error_e gb_err, const uint16_t val) {
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
