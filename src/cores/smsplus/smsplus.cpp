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

static Platform *platform;
static Display *display;
static Core *core;

#define SMS_WIDTH 256
#define SMS_HEIGHT 192
#define SMS_AUD_RATE 44100
#define SMS_FPS 60

// rendering
static uint16_t in_ram(lineBuffer)[2][240];

static uint8_t lineBufferIndex = 0;
static uint8_t framebufferLine[SMS_WIDTH];
static int palette565[32];
static uint8_t sram[0x8000];
static int lcd_line_busy = 0;

// audio
static int audio_buffer[SMS_AUD_RATE / SMS_FPS];

// input
//static uint16_t picoButtons = 0;
static uint16_t smsButtons = 0, smsSystem = 0;

_Noreturn void in_ram(core1_main)();

void in_ram(core1_lcd_draw_line)(uint_fast8_t line, uint_fast8_t index);

union core_cmd {
    struct {
#define CORE_CMD_NOP        0
#define CORE_CMD_LCD_LINE   1
        uint8_t cmd;
        uint8_t line;
        uint8_t index;
    };
    uint32_t full;
};

SMSPlus::SMSPlus(Platform *p) : Core(p, Core::Type::Sms) {
    // crappy
    platform = p;
    display = p->getDisplay();
    core = this;

    // setup audio
    int samples = (int) ((float) SMS_AUD_RATE / SMS_FPS);
    p_platform->getAudio()->setup(SMS_AUD_RATE, samples);

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

bool in_ram(SMSPlus::loop)(uint16_t buttons) {
    //printf("SMSPlus::loop\r\n");
    if (!Core::loop(buttons)) return false;

    smsButtons = 0;
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
    platform->getAudio()->play((void *) &audio_buffer, snd.bufsize);

    return true;
}

extern "C" void in_ram(sms_palette_sync)(int index) {
    palette565[index] = ((bitmap.pal.color[index][0] >> 3) << 11)
                        | ((bitmap.pal.color[index][1] >> 2) << 5)
                        | (bitmap.pal.color[index][2] >> 3);
}

extern "C" void in_ram(sms_render_line)(int line, const uint8_t *buffer) {
    //printf("sms_render_line(%i, %i)\r\n", line, lineBufferIndex);
    // fill line buffer
    for (int i = 8; i < SMS_WIDTH - 8; i++) {
        lineBuffer[lineBufferIndex][i - 8] = palette565[(buffer[i]) & 31];
    }

#ifdef LINUX
    core1_lcd_draw_line(line, lineBufferIndex);
#else
    // wait until previous line is sent
    while (__atomic_load_n(&lcd_line_busy, __ATOMIC_SEQ_CST))
        tight_loop_contents();

    // set core1 in busy state
    __atomic_store_n(&lcd_line_busy, 1, __ATOMIC_SEQ_CST);

    // send to core1
    core_cmd cmd{{CORE_CMD_LCD_LINE, (uint8_t) line, lineBufferIndex}};
    multicore_fifo_push_blocking(cmd.full);
#endif

    // swap line buffer
    lineBufferIndex = !lineBufferIndex;
}

void in_ram(core1_lcd_draw_line)(const uint_fast8_t line, const uint_fast8_t index) {
    //printf("core1_lcd_draw_line(%i, %i)\r\n", line, index);
    if (line == 0) {
        display->setCursor(0, 24);
    }

    // crop line buffer width by 16 pixels (240x240 display)
    for (uint_fast16_t i = 0; i < 240; i++) {
        display->setPixel(lineBuffer[index][i]);
    }

    // signal we are done
    __atomic_store_n(&lcd_line_busy, 0, __ATOMIC_SEQ_CST);

#ifdef LINUX
    if (line == SMS_HEIGHT - 1) {
        display->flip();
    }
#endif
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
