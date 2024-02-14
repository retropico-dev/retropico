//
// Created by cpasjuste on 06/06/23.
//

#include "platform.h"
#include "smsplus.h"

extern "C" {
#include "shared.h"
}

using namespace mb;
using namespace p2d;

static Core *s_core;

#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SMS_AUD_RATE 44100
#define SMS_FPS 60

// rendering
static uint16_t lineBuffer[240];
static uint8_t framebufferLine[SMS_WIDTH];
static int palette565[32];
static uint8_t sram[0x8000];

// audio
static int audio_buffer[SMS_AUD_RATE / SMS_FPS];

// input
static uint16_t smsButtons = 0, smsSystem = 0;

void system_save_sram();

SMSPlus::SMSPlus(const p2d::Display::Settings &ds) : Core(ds, Core::Type::Sms) {
    // crappy
    s_core = this;

    // setup audio
    int samples = (int) ((float) SMS_AUD_RATE / SMS_FPS);
    getAudio()->setup(SMS_AUD_RATE, samples);
}

bool SMSPlus::loadRom(const Io::File &file) {
    printf("SMSPlus::loadRom: %s\r\n", file.getName().c_str());
    m_romName = file.getName();
    m_sramPath = Core::getSavePath(Core::Type::Sms) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";

    auto data = (uint8_t *) file.getPtr();

    memset(framebufferLine, 0x00, SMS_WIDTH);
    memset(sram, 0x00, 0x8000);

    sms.use_fm = 0;
    sms.country = TYPE_OVERSEAS;
    sms.sram = sram;
    sms.dummy = framebufferLine;
    bitmap.data = framebufferLine;
    bitmap.width = 256;
    bitmap.height = 192;
    bitmap.pitch = 256;
    bitmap.depth = 8;

    cart.type = TYPE_SMS;
    // take care of image header, if present
    if ((file.getLength() / 512) & 1) {
        printf("SMSPlus::loadRom: removing rom header...\r\n");
        cart.rom = data + 512;
        cart.pages = ((file.getLength() - 512) / 0x4000);
    } else {
        cart.rom = data;
        cart.pages = (file.getLength() / 0x4000);
    }

    memset(audio_buffer, 0x00, (SMS_AUD_RATE / SMS_FPS));
    system_init(SMS_AUD_RATE);
    system_reset();

    getDisplay()->clear();

    return true;
}

bool in_ram(SMSPlus::loop)() {
    //printf("SMSPlus::loop\r\n");
    if (!Core::loop()) return false;

    smsButtons = 0;
    uint16_t buttons = getInput()->getButtons();
    if (buttons & Input::Button::UP) smsButtons |= INPUT_UP;
    if (buttons & Input::Button::DOWN) smsButtons |= INPUT_DOWN;
    if (buttons & Input::Button::LEFT) smsButtons |= INPUT_LEFT;
    if (buttons & Input::Button::RIGHT) smsButtons |= INPUT_RIGHT;
    if (buttons & Input::Button::B1) smsButtons |= INPUT_BUTTON1;
    if (buttons & Input::Button::B2) smsButtons |= INPUT_BUTTON2;
    if (buttons & Input::Button::START) smsSystem |= INPUT_START;
    if (buttons & Input::Button::SELECT) smsSystem |= INPUT_PAUSE;
    input.pad[0] = smsButtons;
    input.system = smsSystem;

    // process frame
    sms_frame(0);

    // process audio
    for (int x = 0; x < snd.bufsize; x++) {
        audio_buffer[x] = (snd.buffer[0][x] << 16) + snd.buffer[1][x];
        //audio_buffer[x] = ((snd.buffer[0][x] + snd.buffer[1][x]) / 512) + 128;
    }
    getAudio()->play((void *) &audio_buffer, snd.bufsize);

    return true;
}

void SMSPlus::close() {
    printf("SMSPlus::close()\r\n");
    system_save_sram();
}

extern "C" void in_ram(sms_palette_sync)(int index) {
    palette565[index] = ((bitmap.pal.color[index][0] >> 3) << 11)
                        | ((bitmap.pal.color[index][1] >> 2) << 5)
                        | (bitmap.pal.color[index][2] >> 3);
}

extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer) {
    //printf("sms_render_line(%i)\r\n", line);
    // fill line buffer
    for (int i = 8; i < SMS_WIDTH - 8; i++) {
        lineBuffer[i - 8] = palette565[(buffer[i]) & 31];
    }

    if (line == 0) {
        s_core->getDisplay()->setCursor(0, 24);
    }

    s_core->getDisplay()->put(lineBuffer, 240);

#ifdef LINUX
    if (line == SMS_HEIGHT - 1) {
        s_core->getDisplay()->flip();
    }
#endif
}

void system_load_sram(void) {
    printf("system_load_sram: loading sram from %s\r\n", s_core->getSramPath().c_str());

    auto file = Io::File(s_core->getSramPath());
    if (!file.isOpen()) {
        printf("system_load_sram: sram file does not exist, skipping... (%s)\r\n",
               s_core->getSramPath().c_str());
        return;
    }

    if (file.read(0, 0x8000, (char *) sms.sram) != 0x8000) {
        printf("system_load_sram: sram file does not exist, skipping... (%s)\r\n",
               s_core->getSramPath().c_str());
        return;
    }

    printf("system_load_sram: loaded sram file (%s)\r\n", s_core->getSramPath().c_str());
}

void system_save_sram() {
    printf("system_save_sram: saving sram to %s\r\n", s_core->getSramPath().c_str());

    Io::File file{s_core->getSramPath(), Io::File::OpenMode::Write};
    if (!file.isOpen()) {
        printf("system_save_sram: sram file could not be opened for writing, skipping... (%s)\r\n",
               s_core->getSramPath().c_str());
        return;
    }

    int wrote = file.write(0, 0x8000, (const char *) sms.sram);
    if (wrote != 0x8000) {
        printf("system_save_sram: something went wrong when writing to sram file, skipping... (%s)\r\n",
               s_core->getSramPath().c_str());
    } else {
        printf("system_save_sram: saved sram file (%s)\r\n", s_core->getSramPath().c_str());
    }
}
