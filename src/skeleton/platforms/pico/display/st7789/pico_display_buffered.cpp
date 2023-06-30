//
// Created by cpasjuste on 30/05/23.
//

#include <cstdio>
#include "platform.h"
#include "pico_display_buffered.h"
#include "st7789.h"

using namespace mb;

static PicoDisplayBuffered *display;

static int core1_busy = 0;

_Noreturn static void in_ram(core1_main)();

union core_cmd {
    struct {
        uint8_t index;
    };
    uint32_t full;
};

PicoDisplayBuffered::PicoDisplayBuffered() : Display({DISPLAY_WIDTH, DISPLAY_HEIGHT}) {
    printf("PicoDisplay: st7789 pio with double buffering @ %ix%i\r\n", m_size.x, m_size.y);

    // ...
    display = this;

    // init st7789 display
    st7789_init();

    p_pixelBuffer[0] = (uint8_t *) malloc(m_size.x * m_size.y * m_bpp);
    p_pixelBuffer[1] = (uint8_t *) malloc(m_size.x * m_size.y * m_bpp);
    multicore_launch_core1(core1_main);

    // clear the display
    PicoDisplayBuffered::clear();
}

void PicoDisplayBuffered::setCursor(uint16_t x, uint16_t y) {
    m_cursor = {(int16_t) x, (int16_t) y};
}

void PicoDisplayBuffered::setPixel(uint16_t color) {
    *(uint16_t *) (getPixelBuffer(m_bufferIndex) + m_cursor.y * m_pitch + m_cursor.x * m_bpp) = color;
    // emulate tft lcd "put_pixel"
    m_cursor.x++;
    if (m_cursor.x >= m_size.x) {
        m_cursor.x = 0;
        m_cursor.y += 1;
    }
}

void PicoDisplayBuffered::clear(uint16_t color) {
    if (color == Color::Black || color == Color::White) {
        auto buffer = (uint16_t *) getPixelBuffer(m_bufferIndex);
        memset(buffer, color, m_size.x * m_size.y * m_bpp);
    } else {
        auto buffer = getPixelBuffer(m_bufferIndex);
        int size = m_pitch * m_size.y;
        uint64_t color64 = (uint64_t) color << 48;
        color64 |= (uint64_t) color << 32;
        color64 |= (uint64_t) color << 16;
        color64 |= color;
        for (uint_fast16_t i = 0; i < size; i += 8) {
            *(uint64_t *) (buffer + i) = color64;
        }
    }
}

_Noreturn static void in_ram(core1_main)() {
    union core_cmd cmd{};
    uint16_t sizeX = display->getSize().x;
    uint16_t sizeY = display->getSize().y;
    uint16_t pitch = sizeX * 2;
    uint8_t bpp = 2;

    while (true) {
        cmd.full = multicore_fifo_pop_blocking();

        auto buffer = display->getPixelBuffer(cmd.index);
        st7789_set_cursor(0, 0);
        for (uint_fast16_t y = 0; y < sizeY; y++) {
            for (uint_fast16_t x = 0; x < sizeX; x++) {
                st7789_put(*(uint16_t *) (buffer + y * pitch + x * bpp));
            }
        }

        __atomic_store_n(&core1_busy, 0, __ATOMIC_SEQ_CST);
    }
}

void PicoDisplayBuffered::flip() {
    // wait until previous flip on core1 complete
    while (__atomic_load_n(&core1_busy, __ATOMIC_SEQ_CST)) tight_loop_contents();

    __atomic_store_n(&core1_busy, 1, __ATOMIC_SEQ_CST);
    union core_cmd cmd{.index = m_bufferIndex};
    multicore_fifo_push_blocking(cmd.full);
    m_bufferIndex = !m_bufferIndex;
}
