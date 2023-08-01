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
static Display *display;
static Core *core;

#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SMS_AUD_RATE 22050
#define SMS_FPS 60

// rendering
#define LINE_BUFFER_COUNT 64
static uint16_t in_ram(lineBuffer)[LINE_BUFFER_COUNT][240];
static uint8_t lineBufferIndex = 0;
static uint8_t framebufferLine[SMS_WIDTH];
static int palette565[32];
static uint8_t sram[0x8000];

// audio
static int audio_buffer[SMS_AUD_RATE / SMS_FPS];

// input
static uint16_t picoButtons = 0;
static uint16_t smsButtons = 0, smsSystem = 0;

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
    display = p->getDisplay();
    core = this;

    // create saves directory
    p_platform->getIo()->createDir(Io::getSavePath(Core::Type::Sms));

    // setup audio
    int samples = (int) ((float) SMS_AUD_RATE / SMS_FPS);
    p_platform->getAudio()->setup(SMS_AUD_RATE, samples, 2);

    // start Core1, which processes requests to the LCD
    multicore_launch_core1(core1_main);
}

bool SMSPlus::loadRom(Io::FileBuffer file) {
    printf("SMSPlus::loadRom: %s\r\n", file.name);
    m_romName = file.name;
    m_sramPath = Io::getSavePath(Core::Type::Sms) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";
    uint8_t *data = file.data;

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
    if ((file.size / 512) & 1) {
        printf("SMSPlus::loadRom: removing rom header...\r\n");
        cart.rom = data + 512;
        cart.pages = ((file.size - 512) / 0x4000);
    } else {
        cart.rom = data;
        cart.pages = (file.size / 0x4000);
    }

    memset(audio_buffer, 0x00, (SMS_AUD_RATE / SMS_FPS));
    system_init(SMS_AUD_RATE);

    p_platform->getDisplay()->clear();

    return true;
}

bool in_ram(SMSPlus::loop)() {
    //printf("SMSPlus::loop\r\n");

    // process inputs
    picoButtons = platform->getInput()->getButtons();

    // exit requested
    if (picoButtons & Input::Button::START && picoButtons & Input::Button::SELECT
        || picoButtons & mb::Input::Button::QUIT) {
        return false;
    }

    smsButtons = 0;
    if (picoButtons & Input::Button::UP) smsButtons |= INPUT_UP;
    if (picoButtons & Input::Button::DOWN) smsButtons |= INPUT_DOWN;
    if (picoButtons & Input::Button::LEFT) smsButtons |= INPUT_LEFT;
    if (picoButtons & Input::Button::RIGHT) smsButtons |= INPUT_RIGHT;
    if (picoButtons & Input::Button::B1) smsButtons |= INPUT_BUTTON1;
    if (picoButtons & Input::Button::B2) smsButtons |= INPUT_BUTTON2;
    if (picoButtons & Input::Button::START) smsSystem |= INPUT_START;
    if (picoButtons & Input::Button::SELECT) smsSystem |= INPUT_PAUSE;
    input.pad[0] = smsButtons;
    input.system = smsSystem;

    // process frame
    sms_frame(0);

    // process audio
    for (int x = 0; x < snd.bufsize; x++) {
        audio_buffer[x] = (snd.buffer[0][x] << 16) + snd.buffer[1][x];
        //audio_buffer[x] = ((snd.buffer[0][x] + snd.buffer[1][x]) / 512) + 128;
    }
    platform->getAudio()->play((void *) &audio_buffer, snd.bufsize);

    return true;
}

extern "C" void in_ram(sms_palette_sync)(int index) {
    palette565[index] = ((bitmap.pal.color[index][0] >> 3) << 11)
                        | ((bitmap.pal.color[index][1] >> 2) << 5)
                        | (bitmap.pal.color[index][2] >> 3);
}

extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer) {
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
