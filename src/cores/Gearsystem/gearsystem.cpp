//
// Created by cpasjuste on 06/06/23.
//

#include "platform.h"
#include "gearsystem.h"
#include "../external/cores/Gearsystem/src/gearsystem.h"

using namespace mb;

#define SURFACE_BUFFER_SIZE GS_RESOLUTION_MAX_WIDTH * GS_RESOLUTION_MAX_HEIGHT * 2

static GearsystemCore *core;
static Cartridge::ForceConfiguration config;
static s16 audio_buf[GS_AUDIO_BUFFER_SIZE];
static int audio_sample_count = 0;

GearSystem::GearSystem(Platform *p) : Core(p) {
    // create a render surface
    p_surface = new Surface({GS_RESOLUTION_MAX_WIDTH, GS_RESOLUTION_MAX_HEIGHT});
    m_dstPos = {
            (int16_t) ((p_platform->getDisplay()->getSize().x - m_dstSize.x) / 2),
            (int16_t) ((p_platform->getDisplay()->getSize().y - m_dstSize.y) / 2)
    };

    // init audio
    p_platform->getAudio()->setup(44100, 1024, nullptr);

    core = new GearsystemCore();
    core->Init(GS_PIXEL_RGB565);
    config.type = Cartridge::CartridgeNotSupported;
    config.zone = Cartridge::CartridgeUnknownZone;
    config.region = Cartridge::CartridgeUnknownRegion;
    config.system = Cartridge::CartridgeUnknownSystem;

    // boot roms
    //core->GetMemory()->LoadBootromSMS("");
    //core->GetMemory()->LoadBootromGG("");
    core->GetMemory()->EnableBootromSMS(false);
    core->GetMemory()->EnableBootromGG(false);
}

bool GearSystem::loadRom(const std::string &path) {
    size_t size;
    uint8_t *rom = p_platform->getIo()->load("/roms/rom.gg", &size);
    if (!rom) {
        printf("PeanutGB::loadRom: failed to load rom (%s)\r\n", path.c_str());
        return false;
    }

    return loadRom(rom, size);
}

bool GearSystem::loadRom(const uint8_t *buffer, size_t size) {
    if (!core->LoadROMFromBuffer(reinterpret_cast<const u8 *>(buffer), (int) size, &config)) {
        printf("PeanutGB::loadRom: failed to load rom buffer...\r\n");
        return false;
    }

    return true;
}

bool GearSystem::loop() {
    uint32_t buttons = p_platform->getInput()->getButtons();
    if (buttons & mb::Input::Button::QUIT) return false;

    GS_RuntimeInfo runtime_info{};
    core->GetRuntimeInfo(runtime_info);
    if (runtime_info.screen_width != p_surface->getSize().x
        || runtime_info.screen_height != p_surface->getSize().y) {
        printf("GearSystem::loop: screen size changed: %i x %i > %i x %i\r\n",
               p_surface->getSize().x, p_surface->getSize().y,
               runtime_info.screen_width, runtime_info.screen_height);
        delete (p_surface);
        p_surface = new Surface({(int16_t) runtime_info.screen_width,
                                 (int16_t) runtime_info.screen_height},
                                SURFACE_BUFFER_SIZE);
        // update drawing info
        m_dstSize = {(int16_t) ((float) p_surface->getSize().x * 1.33f), p_surface->getSize().y};
        m_dstPos = {
                (int16_t) ((p_platform->getDisplay()->getSize().x - m_dstSize.x) / 2),
                (int16_t) ((p_platform->getDisplay()->getSize().y - m_dstSize.y) / 2)
        };
    }

    // run one frame
    core->RunToVBlank(p_surface->getPixels(), audio_buf, &audio_sample_count);

    // send audio buffer
    if (audio_sample_count > 0) {
        p_platform->getAudio()->play(audio_buf, audio_sample_count);
    }

    // render the surface to the display (scale to 4/3 ar)
    p_platform->getDisplay()->drawSurface(p_surface, m_dstPos, m_dstSize);
    p_platform->getDisplay()->flip();

    return true;
}

GearSystem::~GearSystem() {
    delete (p_surface);
}
