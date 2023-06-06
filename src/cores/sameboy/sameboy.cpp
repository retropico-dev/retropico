//
// Created by cpasjuste on 06/06/23.
//

#include "sameboy.h"
#include "c_wrapper.h"

using namespace mb;

#define MAX_VIDEO_WIDTH 256
#define MAX_VIDEO_HEIGHT 224
#define MAX_VIDEO_PIXELS (MAX_VIDEO_WIDTH * MAX_VIDEO_HEIGHT)

extern GB_gameboy_t gameboy;
extern const unsigned char dmg_boot_bin[];
extern unsigned int dmg_boot_bin_len;

extern Platform *platform;
Surface *surface;

static uint32_t rgb_encode(GB_gameboy_t *gb, uint8_t r, uint8_t g, uint8_t b) {
    return r << 16 | g << 8 | b;
}

static void audio_callback(GB_gameboy_t *gb, GB_sample_t *sample) {

}

static void vblank(GB_gameboy_t *gb, GB_vblank_type_t type) {
    if (type != GB_VBLANK_TYPE_REPEAT) {
        platform->getDisplay()->drawSurface(surface);
        platform->getDisplay()->flip();
    }
}

static void boot_rom_load(GB_gameboy_t *gb, GB_boot_rom_t type) {
    // TODO
    printf("boot_rom_load(%i)\n", type);
    GB_load_boot_rom_from_buffer(gb, dmg_boot_bin, dmg_boot_bin_len);
}

SameBoy::SameBoy(Platform *p) : Core(p) {
    surface = new Surface({MAX_VIDEO_WIDTH, MAX_VIDEO_HEIGHT});

    if (GB_is_inited(&gameboy)) {
        GB_switch_model_and_reset(&gameboy, GB_MODEL_DMG_B);
        //retro_set_memory_maps();
    } else {
        GB_init(&gameboy, GB_MODEL_DMG_B);
    }

    GB_set_border_mode(&gameboy, GB_BORDER_NEVER);
    GB_set_color_correction_mode(&gameboy, GB_COLOR_CORRECTION_DISABLED);
    GB_set_highpass_filter_mode(&gameboy, GB_HIGHPASS_OFF);
    GB_set_turbo_mode(&gameboy, true, false);

    GB_set_boot_rom_load_callback(&gameboy, boot_rom_load);
    GB_set_pixels_output(&gameboy, (uint32_t *) surface->getPixels());
    GB_set_rgb_encode_callback(&gameboy, rgb_encode);
    GB_set_sample_rate(&gameboy, 11025);
    GB_apu_set_sample_callback(&gameboy, audio_callback);
    //GB_set_rumble_callback(&gb, rumble_callback);
    GB_set_vblank_callback(&gameboy, (GB_vblank_callback_t) vblank);
    //GB_set_lcd_status_callback(&gameboy, lcd_status_change);
}

bool SameBoy::loadRom(const uint8_t *buffer, size_t size) {
    GB_load_rom_from_buffer(&gameboy, buffer, size);
    return true;
}

void SameBoy::loop() {
    GB_run_frame(&gameboy);
}
