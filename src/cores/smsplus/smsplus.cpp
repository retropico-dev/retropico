//
// Created by cpasjuste on 06/06/23.
//

#include "platform.h"
#include "smsplus.h"

extern "C" {
#include "shared.h"
}

using namespace retropico;
using namespace p2d;

static Core *s_core;

#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SMS_AUD_RATE 44100
#define SMS_FPS 60

// rendering
static uint8_t screenCropX = 0;
static uint16_t screenBufferLine[240];
static uint8_t smsBufferLine[SMS_WIDTH];
static int palette565[32];
static uint8_t sram[0x8000];

// audio
static int audio_buffer[SMS_AUD_RATE / SMS_FPS];

// input
static uint16_t smsButtons = 0, smsSystem = 0;

void system_save_sram();

void system_load_state();

void system_save_state();

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
    m_sramPath = Core::getSavesPath(Core::Type::Sms) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";
    m_savePath = Utility::replace(m_sramPath, ".srm", ".sav");

    // clear some buffers...
    memset(audio_buffer, 0x00, (SMS_AUD_RATE / SMS_FPS));
    memset(smsBufferLine, 0x00, SMS_WIDTH);
    memset(sram, 0x00, 0x8000);

    cart.type = Utility::endsWith(m_romName, ".gg") ? TYPE_GG : TYPE_SMS;
    if (IS_GG) {
        getDisplay()->setDisplayBounds((int16_t) ((240 - BMP_WIDTH) / 2), (int16_t) ((240 - BMP_HEIGHT) / 2),
                                       BMP_WIDTH, BMP_HEIGHT);
        m_sramPath = Utility::replace(m_sramPath, "/sms", "/gg");
        m_savePath = Utility::replace(m_savePath, "/sms", "/gg");
    } else {
        getDisplay()->setDisplayBounds(0, 24, 240, BMP_HEIGHT);
        screenCropX = 8;
    }

    sms.use_fm = 0;
    sms.country = TYPE_OVERSEAS;
    sms.sram = sram;
    sms.dummy = smsBufferLine;
    bitmap.data = smsBufferLine;
    bitmap.width = BMP_WIDTH;
    bitmap.height = BMP_HEIGHT;
    bitmap.pitch = BMP_WIDTH;
    bitmap.depth = 8;

    // take care of image header, if present
    auto romData = (uint8_t *) file.getPtr();
    if ((file.getLength() / 512) & 1) {
        printf("SMSPlus::loadRom: removing rom header...\r\n");
        cart.rom = romData + 512;
        cart.pages = ((file.getLength() - 512) / 0x4000);
    } else {
        cart.rom = romData;
        cart.pages = (file.getLength() / 0x4000);
    }

    // init sms
    system_init(SMS_AUD_RATE);

    // load state if any
    //system_load_state();

    // initialize
    cpu_reset();
    system_reset();
    system_load_sram();

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
    system_save_state();
}

extern "C" void in_ram(sms_palette_sync)(int index) {
    palette565[index] = ((bitmap.pal.color[index][0] >> 3) << 11)
                        | ((bitmap.pal.color[index][1] >> 2) << 5)
                        | (bitmap.pal.color[index][2] >> 3);
}

extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer) {
    //printf("sms_render_line(%i)\r\n", line);
    for (int i = screenCropX; i < BMP_WIDTH - screenCropX; i++) {
        screenBufferLine[i - screenCropX] = palette565[(buffer[i + BMP_X_OFFSET]) & 31];
    }

    s_core->getDisplay()->put(screenBufferLine, BMP_WIDTH - (screenCropX * 2));

#ifdef LINUX
    if (line == BMP_HEIGHT + BMP_Y_OFFSET - 1) {
        s_core->getDisplay()->flip();
    }
#endif
}

void system_load_sram(void) {
    printf("system_load_sram: loading sram from %s\r\n", s_core->getSramPath().c_str());

    Io::File file(s_core->getSramPath());
    if (!file.isOpen()) {
        printf("system_load_sram: sram file does not exist, skipping... (%s)\r\n",
               s_core->getSramPath().c_str());
        return;
    }

    if (file.read(0, 0x8000, (char *) sms.sram) != 0x8000) {
        printf("system_load_sram: sram file does not exist, skipping... (%s)\r\n",
               s_core->getSramPath().c_str());
    }
}

void system_save_sram() {
    printf("system_save_sram: saving sram to %s\r\n", s_core->getSramPath().c_str());

    Io::File file(s_core->getSramPath(), Io::File::OpenMode::Write);
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

void system_load_state() {
    uint8 reg[0x40];
    uint32_t offset = 0;

    Io::File file(s_core->getSavePath());
    if (!file.isOpen()) {
        printf("system_load_state: state file does not exist, skipping... (%s)\r\n",
               s_core->getSramPath().c_str());
        return;
    }

    /* Initialize everything */
    cpu_reset();
    system_reset();

    /* Load VDP context */
    offset += file.read(offset, sizeof(t_vdp), reinterpret_cast<char *>(&vdp));

    /* Load SMS context */
    offset += file.read(offset, sizeof(t_sms), reinterpret_cast<char *>(&sms));

    /* Load Z80 context */
    offset += file.read(offset, sizeof(Z80_Regs), reinterpret_cast<char *>(Z80_Context));
    offset += file.read(offset, sizeof(int), reinterpret_cast<char *>(&after_EI));

    /* Load YM2413 registers */
    offset += file.read(offset, 0x40, reinterpret_cast<char *>(reg));

    /* Load SN76489 context */
    file.read(offset, sizeof(t_SN76496), reinterpret_cast<char *>(&sn[0]));

    /* Restore callbacks */
    z80_set_irq_callback(sms_irq_callback);

    cpu_readmap[0] = cart.rom + 0x0000; /* 0000-3FFF */
    cpu_readmap[1] = cart.rom + 0x2000;
    cpu_readmap[2] = cart.rom + 0x4000; /* 4000-7FFF */
    cpu_readmap[3] = cart.rom + 0x6000;
    cpu_readmap[4] = cart.rom + 0x0000; /* 0000-3FFF */
    cpu_readmap[5] = cart.rom + 0x2000;
    cpu_readmap[6] = sms.ram;
    cpu_readmap[7] = sms.ram;

    cpu_writemap[0] = sms.dummy;
    cpu_writemap[1] = sms.dummy;
    cpu_writemap[2] = sms.dummy;
    cpu_writemap[3] = sms.dummy;
    cpu_writemap[4] = sms.dummy;
    cpu_writemap[5] = sms.dummy;
    cpu_writemap[6] = sms.ram;
    cpu_writemap[7] = sms.ram;

    sms_mapper_w(3, sms.fcr[3]);
    sms_mapper_w(2, sms.fcr[2]);
    sms_mapper_w(1, sms.fcr[1]);
    sms_mapper_w(0, sms.fcr[0]);

    /* Force full pattern cache update */
    //is_vram_dirty = 1;
    //memset(vram_dirty, 1, 0x200);

    /* Restore palette */
    for (int i = 0; i < PALETTE_SIZE; i += 1) palette_sync(i);
}

void system_save_state() {
    uint8 reg[0x40];
    uint32_t offset = 0;

    Io::File file(s_core->getSavePath(), Io::File::OpenMode::Write);
    if (!file.isOpen()) {
        printf("system_save_state: state file could not be opened for writing, skipping... (%s)\r\n",
               s_core->getSavePath().c_str());
        return;
    }

    /* Save VDP context */
    offset += file.write(offset, sizeof(t_vdp), reinterpret_cast<const char *>(&vdp));

    /* Save SMS context */
    offset += file.write(offset, sizeof(t_sms), reinterpret_cast<const char *>(&sms));

    /* Save Z80 context */
    offset += file.write(offset, sizeof(Z80_Regs), reinterpret_cast<const char *>(Z80_Context));
    offset += file.write(offset, sizeof(int), reinterpret_cast<const char *>(&after_EI));

    /* Save YM2413 registers */
    offset += file.write(offset, 0x40, reinterpret_cast<const char *>(reg));

    /* Save SN76489 context */
    file.write(offset, sizeof(t_SN76496), reinterpret_cast<const char *>(&sn[0]));
}
