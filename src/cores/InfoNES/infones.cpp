//
// Created by cpasjuste on 06/06/23.
//

#include <cstdarg>
#include "platform.h"
#include "infones.h"
#include "InfoNES.h"
#include "InfoNES_System.h"
#include "K6502.h"
#include "cores/Peanut-GB/hedley.h"

using namespace mb;

#define CC(x) ((((x) >> 1) & 15) | ((((x) >> 6) & 15) << 4) | ((((x) >> 11) & 15) << 8))
const WORD in_ram(NesPalette)[64] = {
        CC(0x39ce), CC(0x1071), CC(0x0015), CC(0x2013), CC(0x440e), CC(0x5402), CC(0x5000), CC(0x3c20),
        CC(0x20a0), CC(0x0100), CC(0x0140), CC(0x00e2), CC(0x0ceb), CC(0x0000), CC(0x0000), CC(0x0000),
        CC(0x5ef7), CC(0x01dd), CC(0x10fd), CC(0x401e), CC(0x5c17), CC(0x700b), CC(0x6ca0), CC(0x6521),
        CC(0x45c0), CC(0x0240), CC(0x02a0), CC(0x0247), CC(0x0211), CC(0x0000), CC(0x0000), CC(0x0000),
        CC(0x7fff), CC(0x1eff), CC(0x2e5f), CC(0x223f), CC(0x79ff), CC(0x7dd6), CC(0x7dcc), CC(0x7e67),
        CC(0x7ae7), CC(0x4342), CC(0x2769), CC(0x2ff3), CC(0x03bb), CC(0x0000), CC(0x0000), CC(0x0000),
        CC(0x7fff), CC(0x579f), CC(0x635f), CC(0x6b3f), CC(0x7f1f), CC(0x7f1b), CC(0x7ef6), CC(0x7f75),
        CC(0x7f94), CC(0x73f4), CC(0x57d7), CC(0x5bf9), CC(0x4ffe), CC(0x0000), CC(0x0000), CC(0x0000)};

static Platform *platform;
static Core *core;
static bool stopped = false;
static bool frameLoaded = false;
#define LINE_BUFFER_COUNT 16
static WORD in_ram(lineBufferRGB444)[LINE_BUFFER_COUNT][NES_DISP_WIDTH];
static uint8_t lineBufferIndex = 0;
extern int SpriteJustHit;
static int lcd_line_busy = 0;
// audio
static int16_t in_ram(audio_buffer)[1024];
static int audio_buffer_index = 0;

int InfoNES_LoadSRAM(const std::string &path);

int InfoNES_SaveSRAM(const std::string &path);

_Noreturn void in_ram(core1_main)();

union core_cmd {
    struct {
#define CORE_CMD_NOP        0
#define CORE_CMD_LCD_LINE   1
        uint8_t cmd;
        uint8_t line;
        uint8_t index;
        uint8_t dummy;
    };
    uint32_t full;
};

InfoNES::InfoNES(Platform *p) : Core(p) {
    // crappy
    platform = p;
    core = this;

    // create saves directory
    p_platform->getIo()->createDir(Io::getSavePath(Core::Type::Nes));

    // setup audio
    p_platform->getAudio()->setup(44100, 735, 1);

    // start Core1, which processes requests to the LCD
    multicore_launch_core1(core1_main);
}

bool InfoNES::loadRom(Io::FileBuffer file) {
    printf("InfoNES::loadRom: %s\r\n", file.name);
    m_romName = file.name;
    m_sramPath = Io::getSavePath(Core::Type::Nes) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";
    uint8_t *data = file.data;
    memcpy(&NesHeader, data, sizeof(NesHeader));
    if (memcmp(NesHeader.byID, "NES\x1a", 4) != 0) {
        printf("InfoNES::loadRom: NES header not found in rom...\n");
        return false;
    }

    data += sizeof(NesHeader);
    memset(SRAM, 0, SRAM_SIZE);
    if (NesHeader.byInfo1 & 4) {
        memcpy(&SRAM[0x1000], data, 512);
        data += 512;
    }

    auto romSize = NesHeader.byRomSize * 0x4000;
    ROM = (BYTE *) data;
    data += romSize;

    if (NesHeader.byVRomSize > 0) {
        auto vSize = NesHeader.byVRomSize * 0x2000;
        VROM = (BYTE *) data;
        data += vSize;
    }

    if (InfoNES_Reset() < 0) {
        printf("InfoNES::loadRom: NES reset error\n");
        return false;
    }

    InfoNES_Init();

    // finally, load SRAM if any
    InfoNES_LoadSRAM(m_sramPath);

    p_platform->getDisplay()->clear();

    stopped = false;

    return true;
}

bool in_ram(InfoNES::loop)() {
    if (InfoNES_Menu() == -1) return false;

    while (!frameLoaded && !stopped) {
        if (SpriteJustHit == PPU_Scanline && PPU_ScanTable[PPU_Scanline] == SCAN_ON_SCREEN) {
            int nStep = SPRRAM[SPR_X] * STEP_PER_SCANLINE / NES_DISP_WIDTH;
            K6502_Step(nStep);
            if ((PPU_R1 & R1_SHOW_SP) && (PPU_R1 & R1_SHOW_SCR))
                PPU_R2 |= R2_HIT_SP;
            if ((PPU_R0 & R0_NMI_SP) && (PPU_R1 & R1_SHOW_SP))
                NMI_REQ;
            K6502_Step(STEP_PER_SCANLINE - nStep);
        } else {
            K6502_Step(STEP_PER_SCANLINE);
        }

        FrameStep += STEP_PER_SCANLINE;
        if (FrameStep > STEP_PER_FRAME && FrameIRQ_Enable) {
            FrameStep %= STEP_PER_FRAME;
            IRQ_REQ;
            APU_Reg[0x4015] |= 0x40;
        }

        MapperHSync();
        if (InfoNES_HSync() == -1) {
            printf("InfoNES::loop: InfoNES_HSync failed\r\n");
            return false;
        }
    }

    frameLoaded = false;

    return true;
}

InfoNES::~InfoNES() = default;

void in_ram(core1_lcd_draw_line)(const uint_fast8_t line, const uint_fast8_t index) {
    if (line == 4) {
        platform->getDisplay()->setCursor(0, 4);
    }

    // crop line buffer width by 16 pixels (240x240 display)
    auto display = platform->getDisplay();
    display->drawPixelLine(lineBufferRGB444[index] + 8, 240, Display::Format::RGB444);

    if (line == 235) {
        platform->getDisplay()->flip();
        //platform->getDisplay()->setCursor(0, 0);
    }

    // signal we are done
    //__atomic_store_n(&lcd_line_busy, 0, __ATOMIC_SEQ_CST);
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

    HEDLEY_UNREACHABLE();
}

void in_ram(InfoNES_PreDrawLine)(int line) {
    (void) line;
    //printf("InfoNES_PreDrawLine(%i)\r\n", line);

    // wait until previous line is sent
    //while (__atomic_load_n(&lcd_line_busy, __ATOMIC_SEQ_CST))
    //    tight_loop_contents();

    InfoNES_SetLineBuffer(lineBufferRGB444[lineBufferIndex], NES_DISP_WIDTH);
}

void in_ram(InfoNES_PostDrawLine)(int line) {
    //printf("InfoNES_PostDrawLine(%i)\r\n", line);
#ifdef LINUX
    // we can't draw outside main thread with sdl2
    core1_lcd_draw_line(line, lineBufferIndex);
#else
    // send cmd
    core_cmd cmd{{CORE_CMD_LCD_LINE, (uint8_t) line, lineBufferIndex}};

    //__atomic_store_n(&lcd_line_busy, 1, __ATOMIC_SEQ_CST);
    multicore_fifo_push_blocking(cmd.full);

    // swap line buffer
    lineBufferIndex++;
    if (lineBufferIndex == LINE_BUFFER_COUNT) {
        lineBufferIndex = 0;
    }
#endif
}

void in_ram(InfoNES_LoadFrame)() {
    //printf("InfoNES_LoadFrame\r\n");
    frameLoaded = true;
}

int InfoNES_ReadRom(const char *pszFileName) {
    printf("InfoNES_ReadRom(%s)\r\n", pszFileName);
    return 0;
}

void InfoNES_ReleaseRom() {
    printf("InfoNES_ReleaseRom\r\n");
    // memory leak on linux, we don't care...
    //ROM = nullptr;
    //VROM = nullptr;
    stopped = true;
}

void in_ram(InfoNES_PadState)(DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem) {
    static constexpr int LEFT = 1 << 6;
    static constexpr int RIGHT = 1 << 7;
    static constexpr int UP = 1 << 4;
    static constexpr int DOWN = 1 << 5;
    static constexpr int SELECT = 1 << 2;
    static constexpr int START = 1 << 3;
    static constexpr int A = 1 << 0;
    static constexpr int B = 1 << 1;
    (void) pdwPad1;
    (void) pdwSystem;

    uint16_t buttons = platform->getInput()->getButtons();

    // exit requested
    if (buttons & Input::Button::START && buttons & Input::Button::SELECT
        || buttons & mb::Input::Button::QUIT) {
        InfoNES_SaveSRAM(core->getSramPath());
        InfoNES_Fin();
        return;
    }

    // emulation inputs
    auto &pad = *pdwPad1;
    pad = (buttons & mb::Input::Button::LEFT ? LEFT : 0) |
          (buttons & mb::Input::Button::RIGHT ? RIGHT : 0) |
          (buttons & mb::Input::Button::UP ? UP : 0) |
          (buttons & mb::Input::Button::DOWN ? DOWN : 0) |
          (buttons & mb::Input::Button::B1 ? A : 0) |
          (buttons & mb::Input::Button::B2 ? B : 0) |
          (buttons & mb::Input::Button::SELECT ? SELECT : 0) |
          (buttons & mb::Input::Button::START ? START : 0) |
          0;
}

int InfoNES_Menu() {
    //printf("InfoNES_Menu\r\n");
    return stopped ? -1 : 0;
}

void InfoNES_SoundInit() {
    printf("InfoNES_SoundInit\r\n");
}

int InfoNES_SoundOpen(int samples_per_sync, int sample_rate) {
    printf("InfoNES_SoundOpen(%i, %i)\r\n", samples_per_sync, sample_rate);
    return 1;
}

void InfoNES_SoundClose() {
}

int in_ram(InfoNES_GetSoundBufferSize)() {
    //printf("InfoNES_GetSoundBufferSize\r\n");
    return 735 * sizeof(uint8_t);
}

void in_ram(InfoNES_SoundOutput)(int samples, BYTE *w1, BYTE *w2, BYTE *w3, BYTE *w4, BYTE *w5) {
    //printf("InfoNES_SoundOutput: samples = %i\r\n", samples);
    uint8_t byte;
    int32_t sample;

    for (uint_fast32_t i = 0; i < samples; i++) {
        byte = (w1[i] + w2[i] + w3[i] + w4[i] + w5[i]) / 5;
        sample = (byte - 128) * 256;
        if (sample > 32767) sample = 32767;
        else if (sample < -32768) sample = -32768;
        audio_buffer[audio_buffer_index] = (int16_t) sample;
        audio_buffer_index++;
        if (audio_buffer_index >= 735) {
            platform->getAudio()->play(audio_buffer, audio_buffer_index);
            audio_buffer_index = 0;
        }
    }
}

void InfoNES_MessageBox(const char *pszMsg, ...) {
    printf("[InfoNES]");
    va_list args;
    va_start(args, pszMsg);
    vprintf(pszMsg, args);
    va_end(args);
    printf("\n");
}

static int nSRAM_SaveFlag;

int InfoNES_LoadSRAM(const std::string &path) {
    Io::FileBuffer fileBuffer;
    unsigned char chData;
    unsigned char chTag;
    int nRunLen;
    int nDecoded;
    int nDecLen;
    int nIdx;

    nSRAM_SaveFlag = 0;

    if (!ROM_SRAM) {
        printf("InfoNES_LoadSRAM: game does not support SRAM, skipping...\r\n");
        return 1;
    }

    nSRAM_SaveFlag = 1;

    fileBuffer = platform->getIo()->read(path, Io::Target::Flash);
    if (!fileBuffer.data) {
        printf("InfoNES_LoadSRAM: could not load SRAM: invalid file (%s)\r\n", path.c_str());
        return -1;
    } else if (fileBuffer.size != SRAM_SIZE) {
        printf("InfoNES_LoadSRAM: could not load SRAM: invalid file size (%s)\r\n", path.c_str());
#if LINUX
        free(fileBuffer.data);
#endif
        return -1;
    }

    nDecoded = 0;
    nDecLen = 0;
    chTag = fileBuffer.data[nDecoded++];

    while (nDecLen < 8192) {
        chData = fileBuffer.data[nDecoded++];
        if (chData == chTag) {
            chData = fileBuffer.data[nDecoded++];
            nRunLen = fileBuffer.data[nDecoded++];
            for (nIdx = 0; nIdx < nRunLen + 1; ++nIdx) {
                SRAM[nDecLen++] = chData;
            }
        } else {
            SRAM[nDecLen++] = chData;
        }
    }

    printf("InfoNES_LoadSRAM: loaded SRAM from %s\r\n", path.c_str());

    return 1;
}

int InfoNES_SaveSRAM(const std::string &path) {
    unsigned char pDstBuf[SRAM_SIZE];
    Io::FileBuffer fileBuffer;
    int nUsedTable[256];
    unsigned char chData;
    unsigned char chPrevData;
    unsigned char chTag;
    int nIdx;
    int nEncoded;
    int nEncLen;
    int nRunLen;

    if (!nSRAM_SaveFlag)
        return 0;

    memset(nUsedTable, 0, sizeof nUsedTable);

    for (nIdx = 0; nIdx < SRAM_SIZE; ++nIdx) {
        ++nUsedTable[SRAM[nIdx++]];
    }
    for (nIdx = 1, chTag = 0; nIdx < 256; ++nIdx) {
        if (nUsedTable[nIdx] < nUsedTable[chTag])
            chTag = nIdx;
    }

    nEncoded = 0;
    nEncLen = 0;
    nRunLen = 1;

    pDstBuf[nEncLen++] = chTag;

    chPrevData = SRAM[nEncoded++];

    while (nEncoded < SRAM_SIZE && nEncLen < SRAM_SIZE - 133) {
        chData = SRAM[nEncoded++];

        if (chPrevData == chData && nRunLen < 256)
            ++nRunLen;
        else {
            if (nRunLen >= 4 || chPrevData == chTag) {
                pDstBuf[nEncLen++] = chTag;
                pDstBuf[nEncLen++] = chPrevData;
                pDstBuf[nEncLen++] = nRunLen - 1;
            } else {
                for (nIdx = 0; nIdx < nRunLen; ++nIdx)
                    pDstBuf[nEncLen++] = chPrevData;
            }

            chPrevData = chData;
            nRunLen = 1;
        }

    }
    if (nRunLen >= 4 || chPrevData == chTag) {
        pDstBuf[nEncLen++] = chTag;
        pDstBuf[nEncLen++] = chPrevData;
        pDstBuf[nEncLen++] = nRunLen - 1;
    } else {
        for (nIdx = 0; nIdx < nRunLen; ++nIdx)
            pDstBuf[nEncLen++] = chPrevData;
    }

    fileBuffer.data = pDstBuf;
    fileBuffer.size = SRAM_SIZE;

    printf("InfoNES_SaveSRAM: saving SRAM to %s\r\n", path.c_str());

    return platform->getIo()->write(path, fileBuffer) == true;
}
