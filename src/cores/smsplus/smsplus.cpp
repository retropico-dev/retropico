//
// Created by cpasjuste on 06/06/23.
//

#include "platform.h"
#include "smsplus.h"

extern "C" {
#include "shared.h"
}

using namespace mb;

static Platform *platform;
static Core *core;

#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SND_RATE 22050

#define LINE_BUFFER_COUNT 16
static uint16_t in_ram(lineBuffer)[LINE_BUFFER_COUNT][240];
static uint8_t lineBufferIndex = 0;

static int palette565[32];
static uint8_t framebufferLine[256];
static uint8_t sram[0x8000];

_Noreturn void in_ram(core1_main)();

void in_ram(core1_lcd_draw_line)(uint_fast8_t line, uint_fast8_t index);

union core_cmd {
    struct {
#define CORE_CMD_NOP        0
#define CORE_CMD_LCD_LINE   1
#define CORE_CMD_LCD_FLIP   2
        uint8_t cmd;
        uint8_t line;
        uint8_t index;
    };
    uint32_t full;
};

SMSPlus::SMSPlus(Platform *p) : Core(p) {
    // crappy
    platform = p;
    core = this;

    // create saves directory
    p_platform->getIo()->createDir(Io::getSavePath(Core::Type::Sms));

    // setup audio
    p_platform->getAudio()->setup(22050, 735, 2);

    // start Core1, which processes requests to the LCD
    multicore_launch_core1(core1_main);
}

bool SMSPlus::loadRom(const std::string &path) {
    auto file = p_platform->getIo()->read(path, Io::Target::FlashRomData);
    if (!file.data) {
        printf("SMSPlus::loadRom: failed to load rom (%s)\r\n", path.c_str());
        return false;
    }

    m_romPath = path;
    m_sramPath = Io::getSavePath(Core::Type::Sms) + "/"
                 + Utility::removeExt(Utility::baseName(m_romPath)) + ".srm";

    return loadRom(file);
}

bool SMSPlus::loadRom(Io::FileBuffer file) {
    uint8_t *data = file.data;

    memset(framebufferLine, 0x00, 256);
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

    cart.pages = (file.size / 0x4000);
    cart.rom = data;
    cart.type = TYPE_SMS;

    system_init(SND_RATE);

    p_platform->getDisplay()->clear();

    return true;
}

bool in_ram(SMSPlus::loop)() {
    //printf("SMSPlus::loop\r\n");
    uint16_t smsButtons = 0, smsSystem = 0;

    // process inputs
    uint16_t buttons = platform->getInput()->getButtons();
    if (buttons & Input::QUIT) return false;

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

    return true;
}

extern "C" void sms_palette_sync(int index) {
    palette565[index] = ((bitmap.pal.color[index][0] >> 3) << 11)
                        | ((bitmap.pal.color[index][1] >> 2) << 5)
                        | (bitmap.pal.color[index][2] >> 3);
}

extern "C" void sms_render_line(int line, const uint8_t *buffer) {
    union core_cmd cmd{};
    cmd.cmd = CORE_CMD_LCD_LINE;
    cmd.line = line;
    cmd.index = lineBufferIndex;

    for (int i = 8; i < SMS_WIDTH - 8; i++) {
        uint16_t pixel = palette565[(buffer[i]) & 31];
        lineBuffer[lineBufferIndex][i - 8] = pixel;
    }

    // swap line buffer
    lineBufferIndex++;
    if (lineBufferIndex == LINE_BUFFER_COUNT) {
        lineBufferIndex = 0;
    }

#ifdef LINUX
    core1_lcd_draw_line(cmd.line, cmd.index);
#else
    multicore_fifo_push_blocking(cmd.full);
#endif
}

void in_ram(core1_lcd_draw_line)(const uint_fast8_t line, const uint_fast8_t index) {
    //printf("core1_lcd_draw_line(%i, %i)\r\n", line, index);
    auto display = platform->getDisplay();

    if (line == 0) {
        display->setCursor(0, 24);
    }

    // crop line buffer width by 16 pixels (240x240 display)
    display->drawPixelLine(lineBuffer[index], 240);

    if (line == SMS_HEIGHT - 1) {
        display->flip();
    }
}

_Noreturn void in_ram(core1_main)() {
    union core_cmd cmd{};

    while (true) {
        cmd.full = multicore_fifo_pop_blocking();
        switch (cmd.cmd) {
            case CORE_CMD_LCD_LINE:
                core1_lcd_draw_line(cmd.line, cmd.index);
                break;
            case CORE_CMD_NOP:
            default:
                break;
        }
    }
}
