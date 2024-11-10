//
// Created by cpasjuste on 06/06/23.
//

#include "platform.h"
#include "sameboy.h"
//#include "c_wrapper.h"
#include "Core/gb.h"

using namespace retropico;
using namespace p2d;

static Core *s_core;

//#define MAX_VIDEO_WIDTH 256
//#define MAX_VIDEO_HEIGHT 224
#define MAX_VIDEO_WIDTH 160
#define MAX_VIDEO_HEIGHT 144
#define MAX_VIDEO_PIXELS (MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT)

//extern GB_gameboy_t gameboy;
GB_gameboy_t gameboy;
extern const unsigned char dmg_boot_bin[];
extern unsigned int dmg_boot_bin_len;

Surface *surface;

uint32_t in_ram(active_pixel_buffer[160 * 144]);

// audio
//static int audio_buffer[SMS_AUD_RATE / SMS_FPS];

static uint32_t rgb_encode(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b) {
    return r << 16 | g << 8 | b;
    /*
    uint16_t B = (uint16_t) ((b >> 3) << 0);
    uint16_t G = (uint16_t) ((g >> 2) << 5);
    uint16_t R = (uint16_t) ((r >> 3) << 11);
    return (uint32_t) (R | G | B);
    */
}

static void audio_callback(GB_gameboy_t *gb, GB_sample_t *sample) {
}

static void vblank(GB_gameboy_t *gb, GB_vblank_type_t type) {
    if (type != GB_VBLANK_TYPE_REPEAT) {
        //s_core->getDisplay()->drawSurface(surface);
        //s_core->getDisplay()->flip();
        //printf("display\n");

        /*
        uint32_t pixel = 0;

        for (uint32_t y = 0; y < 144; y++) {
#if 0
            s_core->getDisplay()->setCursor(0, y);
            s_core->getDisplay()->putFast((uint16_t *) &active_pixel_buffer[160 * y], 160);
#else
            s_core->getDisplay()->setCursor(0, y);
            for (uint32_t x = 0; x < 160; x++) {
                s_core->getDisplay()->put((uint16_t) active_pixel_buffer[pixel++]);
            }
#endif
        }
    */
    }
}

static void boot_rom_load(GB_gameboy_t *gb, GB_boot_rom_t type) {
    printf("boot_rom_load(%i)\n", type);
    GB_load_boot_rom_from_buffer(gb, dmg_boot_bin, dmg_boot_bin_len);
}

SameBoy::SameBoy(const p2d::Display::Settings &ds) : Core(ds, Core::Type::Gb) {
    // crappy
    s_core = this;

    // setup audio
    //int samples = (int) ((float) SMS_AUD_RATE / SMS_FPS);
    //getAudio()->setup(SMS_AUD_RATE, samples);

    //surface = new Surface({MAX_VIDEO_WIDTH, MAX_VIDEO_HEIGHT});

    if (GB_is_inited(&gameboy)) {
        GB_switch_model_and_reset(&gameboy, GB_MODEL_DMG_B);
    } else {
        GB_init(&gameboy, GB_MODEL_DMG_B);
    }

    GB_set_palette(&gameboy, &GB_PALETTE_MGB);
    GB_set_border_mode(&gameboy, GB_BORDER_NEVER);
    GB_set_color_correction_mode(&gameboy, GB_COLOR_CORRECTION_DISABLED);
    GB_set_highpass_filter_mode(&gameboy, GB_HIGHPASS_OFF);
    GB_set_turbo_mode(&gameboy, true, false);

    GB_set_boot_rom_load_callback(&gameboy, boot_rom_load);
    //GB_set_pixels_output(&gameboy, (uint32_t *) surface->getPixels());
    GB_set_pixels_output(&gameboy, active_pixel_buffer);
    GB_set_rgb_encode_callback(&gameboy, rgb_encode);
    GB_set_sample_rate(&gameboy, 11025);
    GB_apu_set_sample_callback(&gameboy, audio_callback);
    //GB_set_rumble_callback(&gb, rumble_callback);
    GB_set_vblank_callback(&gameboy, (GB_vblank_callback_t) vblank);
    //GB_set_lcd_status_callback(&gameboy, lcd_status_change);
}

static size_t rounded_rom_size(size_t size) {
    size = (size + 0x3FFF) & ~0x3FFF; /* Round to bank */
    /* And then round to a power of two */
    while (size & (size - 1)) {
        /* I promise this works. */
        size |= size >> 1;
        size++;
    }
    if (size < 0x8000) {
        size = 0x8000;
    }
    return size;
}

bool SameBoy::loadRom(const Io::File &file) {
    printf("SameBoy::loadRom: %s\r\n", file.getName().c_str());
    m_romName = file.getName();
    m_sramPath = Core::getSavesPath(Core::Type::Gb) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";
    m_savePath = Utility::replace(m_sramPath, ".srm", ".sav");

    // clear some buffers...
    //memset(audio_buffer, 0x00, (SMS_AUD_RATE / SMS_FPS));

    // load rom
    //GB_load_rom_from_buffer(&gameboy, file.getPtr(), file.getLength());
    gameboy.rom_size = rounded_rom_size(file.getLength());
    gameboy.rom = const_cast<uint8_t *>(file.getPtr());
    GB_configure_cart(&gameboy);
    gameboy.tried_loading_sgb_border = false;
    gameboy.has_sgb_border = false;

    return true;
}

bool in_ram(SameBoy::loop)() {
    //printf("SameBoy::loop\r\n");
    if (!Core::loop()) return false;

    GB_run_frame(&gameboy);

    /*
    // process audio
    for (int x = 0; x < snd.bufsize; x++) {
        audio_buffer [x] = (snd.buffer[0][x] << 16) + snd.buffer[1][x];
        //audio_buffer[x] = ((snd.buffer[0][x] + snd.buffer[1][x]) / 512) + 128;
    }
    getAudio()->play((void *) &audio_buffer, snd.bufsize);
    */

    return true;
}

void SameBoy::close() {
    printf("SameBoy::close()\r\n");
}
