//
// Created by cpasjuste on 06/06/23.
//

#ifdef LINUX
#include "platform_linux.h"
#else
#include "platform_pico.h"
#endif

#include "peanut-gb.h"
#include "gbcolors.h"
#include "hedley.h"
extern "C" {
#define ENABLE_SOUND 1
#include "minigb_apu.h"
#include "peanut_gb.h"
};

using namespace mb;

#ifdef ENABLE_RAM_BANK
static unsigned char rom_bank0[65536];
#endif
static const uint8_t *gb_rom = nullptr;
static uint8_t gb_ram[32768];
static int lcd_line_busy = 0;

static palette_t palette;
static uint8_t manual_palette_selected = 0;

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
    Platform *platform;
    Surface *surface;
};
static struct gb_priv gb_priv{};

/* Pixel data is stored in here. */
static uint8_t pixels_buffer[LCD_WIDTH];

static struct gb_s gameboy;

uint8_t gb_rom_read(struct gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
#ifdef ENABLE_RAM_BANK
    if (addr < sizeof(rom_bank0)) {
        return rom_bank0[addr];
    }
#endif

    return gb_rom[addr];
}

uint8_t gb_cart_ram_read(struct gb_s *gb, const uint_fast32_t addr) {
    (void) gb;
    return gb_ram[addr];
}

void gb_cart_ram_write(struct gb_s *gb, const uint_fast32_t addr, const uint8_t val) {
    (void) gb;
    gb_ram[addr] = val;
}

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

static bool m_scale = false;

void core1_lcd_draw_line(const uint_fast8_t line) {
    // write a line to display buffer
    mb::Utility::Vec2i pos{};

    if (m_scale) {
        for (uint_fast8_t x = 0; x < LCD_WIDTH; x++) {
            gb_priv.surface->setPixel(x, line,
                                      palette[(pixels_buffer[x] & LCD_PALETTE_ALL) >> 4][pixels_buffer[x] & 3]);
        }
    } else {
        pos.x = (int16_t) ((gb_priv.platform->getDisplay()->getSize().x - LCD_WIDTH) / 2);
        pos.y = (int16_t) ((gb_priv.platform->getDisplay()->getSize().y - LCD_HEIGHT) / 2);
        for (uint_fast8_t x = 0; x < LCD_WIDTH; x++) {
            uint16_t p = palette[(pixels_buffer[x] & LCD_PALETTE_ALL) >> 4][pixels_buffer[x] & 3];
            gb_priv.platform->getDisplay()->drawPixel({(int16_t) (x + pos.x), (int16_t) (line + pos.y)}, p);
        }
    }

    // flip
    if (line == LCD_HEIGHT - 1) {
        if (m_scale) {
            gb_priv.platform->getDisplay()->drawSurface(gb_priv.surface, {0, 0},
                                                        gb_priv.platform->getDisplay()->getSize());
        }
        // testing
        //gb_priv.platform->getDisplay()->setRotation(1);
        //gb_priv.platform->getDisplay()->fillRect(16, 16, 64, 32, mb::Display::Color::Yellow);
        //gb_priv.platform->getDisplay()->setCursor(4, 4);
        //gb_priv.platform->getDisplay()->print("Hello World");
        // testing
        gb_priv.platform->getDisplay()->flip();
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

PeanutGB::PeanutGB(Platform *p) : Core(p) {
    // create a render surface
    p_surface = new mb::Surface({LCD_WIDTH, LCD_HEIGHT});

    // init audio
    p_platform->getAudio()->setup(AUDIO_SAMPLE_RATE, AUDIO_SAMPLES, audio_callback);
    audio_init();
}

bool PeanutGB::loadRom(const std::string &path) {
    uint8_t *rom = p_platform->getIo()->load("/roms/rom.gb");
    if (!rom) {
        printf("PeanutGB::loadRom: failed to load rom (%s)\r\n", path.c_str());
        return false;
    }

    return loadRom(rom, 0);
}

bool PeanutGB::loadRom(const uint8_t *buffer, size_t size) {
    enum gb_init_error_e ret;

    gb_rom = buffer;
#ifdef ENABLE_RAM_BANK
    memcpy(rom_bank0, buffer, sizeof(rom_bank0));
#endif

    // start Core1, which processes requests to the LCD
    multicore_launch_core1(main_core1);

    // initialise GB context
    gb_priv.platform = p_platform;
    gb_priv.surface = p_surface;
    ret = gb_init(&gameboy, &gb_rom_read, &gb_cart_ram_read, &gb_cart_ram_write, &gb_error, &gb_priv);

    if (ret != GB_INIT_NO_ERROR) {
        printf("error: %d\r\n", ret);
        stdio_flush();
        while (true) { __wfi(); }
        HEDLEY_UNREACHABLE();
    }

    // automatically assign a colour palette to the game
    char rom_title[16];
    auto_assign_palette(palette, gb_colour_hash(&gameboy), gb_get_rom_name(&gameboy, rom_title));

    gb_init_lcd(&gameboy, &lcd_draw_line);
    //gb.direct.interlace = 1;

    return true;
}

bool PeanutGB::loop() {
    gameboy.gb_frame = 0;

    do {
        __gb_step_cpu(&gameboy);
        tight_loop_contents();
    } while (HEDLEY_LIKELY(gameboy.gb_frame == 0));

    /* Required since we do not know whether a button remains
     * pressed over a serial connection. */
    //if (frames % 4 == 0) gameboy.direct.joypad = 0xFF;
    gameboy.direct.joypad = 0xFF;

    // handle input
    uint16_t buttons = p_platform->getInput()->getButtons();
    if (buttons > 0 && !(buttons & mb::Input::Button::DELAY)) {
        // exit requested (linux)
        if (buttons & mb::Input::Button::QUIT) return false;

        // emulation inputs
        gameboy.direct.joypad_bits.a = !(buttons & mb::Input::Button::B1);
        gameboy.direct.joypad_bits.b = !(buttons & mb::Input::Button::B2);
        gameboy.direct.joypad_bits.select = !(buttons & mb::Input::Button::SELECT);
        gameboy.direct.joypad_bits.start = !(buttons & mb::Input::Button::START);
        gameboy.direct.joypad_bits.up = !(buttons & mb::Input::Button::UP);
        gameboy.direct.joypad_bits.right = !(buttons & mb::Input::Button::RIGHT);
        gameboy.direct.joypad_bits.down = !(buttons & mb::Input::Button::DOWN);
        gameboy.direct.joypad_bits.left = !(buttons & mb::Input::Button::LEFT);

        // hotkey / combos
        if (buttons & mb::Input::Button::SELECT) {
            p_platform->getInput()->setRepeatDelay(INPUT_DELAY_UI);
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
            p_platform->getInput()->setRepeatDelay(0);
        }
    }

    return true;
}

PeanutGB::~PeanutGB() {
    delete (p_surface);
}
