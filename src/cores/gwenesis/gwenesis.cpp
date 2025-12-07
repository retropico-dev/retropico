//
// Created by cpasjuste on 06/06/23.
//

#include "platform.h"
#include "gwenesis.h"

extern "C" {
#include "gwenesis_savestate.h"
#include "gwenesis_sn76489.h"
#include "gwenesis_vdp.h"
#include "bus/gwenesis_bus.h"
#include "m68k.h"
#include "ym2612.h"
#include "z80inst.h"
}

using namespace retropico;
using namespace p2d;

static Core *s_core;

int16_t gwenesis_sn76489_buffer[GWENESIS_AUDIO_BUFFER_LENGTH_PAL];
int sn76489_index;
int sn76489_clock;

int16_t gwenesis_ym2612_buffer[GWENESIS_AUDIO_BUFFER_LENGTH_PAL];
int ym2612_index;
int ym2612_clock;

volatile unsigned int current_frame;
volatile unsigned int frame_counter;
unsigned int lines_per_frame = LINES_PER_FRAME_NTSC; //262; /* NTSC: 262, PAL: 313 */
unsigned int scan_line;
unsigned int drawFrame = 1;

// system clock is video clock
int system_clock;

extern unsigned char gwenesis_vdp_regs[0x20];
extern unsigned int gwenesis_vdp_status;
extern unsigned char *screen;

extern unsigned int screen_width, screen_height;
static int hori_screen_offset, vert_screen_offset;
int hint_counter;
extern int hint_pending;

Gwenesis::Gwenesis(const Display::Settings &displaySettings) : Core(displaySettings, Genesis) {
    // crappy
    s_core = this;
}

bool Gwenesis::loadRom(const Io::File &file) {
    printf("Gwenesis::loadRom: %s\r\n", file.getName().c_str());
    m_romName = file.getName();
    m_sramPath = getSavesPath(Genesis) + "/"
                 + Utility::removeExt(Utility::baseName(m_romName)) + ".srm";
    m_savePath = Utility::replace(m_sramPath, ".srm", ".sav");

    //gwenesis_sn76489_buffer = static_cast<int16_t *>(malloc(GWENESIS_AUDIO_BUFFER_LENGTH_NTSC * 2 * sizeof(int16_t)));
    memset(gwenesis_sn76489_buffer, 0, GWENESIS_AUDIO_BUFFER_LENGTH_PAL);

    load_cartridge(const_cast<uint8_t *>(file.getPtr()), file.getLength());
    power_on();
    reset_emulation();

    return true;
}

bool in_ram(Gwenesis::loop)() {
    //printf("Gwenesis::loop\r\n");
    if (!Core::loop()) return false;

    current_frame = frame_counter;
    hint_counter = gwenesis_vdp_regs[10];

    screen_height = REG1_PAL ? 240 : 224;
    screen_width = REG12_MODE_H40 ? 320 : 256;
    lines_per_frame = REG1_PAL ? LINES_PER_FRAME_PAL : LINES_PER_FRAME_NTSC;
    vert_screen_offset = REG1_PAL ? 0 : 320 * (240 - 224) / 2;

    hori_screen_offset = 0; //REG12_MODE_H40 ? 0 : (320 - 256) / 2;
    screen = getDisplay()->getFramebuffer()->getPixels();
    gwenesis_vdp_set_buffer(reinterpret_cast<unsigned short *>(screen[vert_screen_offset + hori_screen_offset]));

    gwenesis_vdp_render_config();

    /* Reset the difference clocks and audio index */
    system_clock = 0;
    zclk = 0;

    ym2612_clock = 0;
    ym2612_index = 0;

    sn76489_clock = 0;
    sn76489_index = 0;

    scan_line = 0;

    while (scan_line < lines_per_frame) {
        m68k_run(system_clock + VDP_CYCLES_PER_LINE);
        z80_run(system_clock + VDP_CYCLES_PER_LINE);

        if (drawFrame) gwenesis_vdp_render_line(scan_line);

        // On these lines, the line counter interrupt is reloaded
        if ((scan_line == 0) || (scan_line > screen_height)) {
            //  if (REG0_LINE_INTERRUPT != 0)
            //    printf("HINTERRUPT counter reloaded: (scan_line: %d, new
            //    counter: %d)\n", scan_line, REG10_LINE_COUNTER);
            hint_counter = REG10_LINE_COUNTER;
        }

        // interrupt line counter
        if (--hint_counter < 0) {
            if ((REG0_LINE_INTERRUPT != 0) && (scan_line <= screen_height)) {
                hint_pending = 1;
                // printf("Line int pending %d\n",scan_line);
                if ((gwenesis_vdp_status & STATUS_VIRQPENDING) == 0)
                    m68k_update_irq(4);
            }
            hint_counter = REG10_LINE_COUNTER;
        }

        scan_line++;

        // vblank begin at the end of last rendered line
        if (scan_line == screen_height) {
            if (REG1_VBLANK_INTERRUPT != 0) {
                // printf("IRQ VBLANK\n");
                gwenesis_vdp_status |= STATUS_VIRQPENDING;
                m68k_set_irq(6);
            }
            z80_irq_line(1);
        }
        if (scan_line == (screen_height + 1)) {
            z80_irq_line(0);
        }

        system_clock += VDP_CYCLES_PER_LINE;
    }

    if (GWENESIS_AUDIO_ACCURATE == 1) {
        gwenesis_SN76489_run(system_clock);
        ym2612_run(system_clock);
    }

    // reset m68k cycles to the begin of next frame cycle
    m68k.cycles -= system_clock;

    // copy audio samples
    //gwenesis_sound_submit();

    return true;
}

void Gwenesis::close() {
    printf("Gwenesis::close()\r\n");
}

extern "C" void gwenesis_io_get_buttons() {
}
