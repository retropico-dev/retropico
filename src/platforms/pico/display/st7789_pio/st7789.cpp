//
// Created by cpasjuste on 08/06/23.
//

#include <hardware/gpio.h>
#include <pico/time.h>
#include "st7789_lcd.pio.h"
#include "st7789.h"

static PIO m_pio = pio0;
static uint m_sm = 0;

// Format: cmd length (including cmd byte), post delay in units of 5 ms, then cmd payload
// Note the delays have been shortened a little
static const uint8_t st7789_init_seq[] = {
        1, 20, 0x01,                        // Software reset
        1, 10, 0x11,                        // Exit sleep mode
        2, 2, 0x3a, 0x55,                   // Set colour mode to 16 bit
        2, 0, 0x36, 0x00,                   // Set MADCTL: row then column, refresh is bottom to top ????
        5, 0, 0x2a, 0x00, 0x00, DISPLAY_WIDTH >> 8, DISPLAY_WIDTH & 0xff,   // CASET: column addresses
        5, 0, 0x2b, 0x00, 0x00, DISPLAY_HEIGHT >> 8, DISPLAY_HEIGHT & 0xff, // RASET: row addresses
        //1, 0, 0xC6, 0x0F,    // Frame rate control in normal mode (Default value (60HZ))
        1, 2, 0x21,                         // Inversion on, then 10 ms delay (supposedly a hack?)
        1, 2, 0x13,                         // Normal display on, then 10 ms delay
        1, 2, 0x29,                         // Main screen turn on, then wait 500 ms
        0                                   // Terminate list
};

static inline void st7789_lcd_set_dc_cs(bool dc, bool cs) {
    sleep_us(1);
    gpio_put_masked((1u << PIN_DC) | (1u << PIN_CS), !!dc << PIN_DC | !!cs << PIN_CS);
    sleep_us(1);
}

static inline void st7789_lcd_write_cmd(const uint8_t *cmd, size_t count) {
    st7789_lcd_wait_idle(m_pio, m_sm);
    st7789_lcd_set_dc_cs(false, false);
    st7789_lcd_put(m_pio, m_sm, *cmd++);
    if (count >= 2) {
        st7789_lcd_wait_idle(m_pio, m_sm);
        st7789_lcd_set_dc_cs(true, false);
        for (size_t i = 0; i < count - 1; ++i)
            st7789_lcd_put(m_pio, m_sm, *cmd++);
    }
    st7789_lcd_wait_idle(m_pio, m_sm);
    st7789_lcd_set_dc_cs(true, true);
}

static inline void st7789_lcd_write_cmd(uint8_t cmd) {
    st7789_lcd_wait_idle(m_pio, m_sm);
    st7789_lcd_set_dc_cs(false, false);
    st7789_lcd_put(m_pio, m_sm, cmd);
    st7789_lcd_wait_idle(m_pio, m_sm);
    st7789_lcd_set_dc_cs(true, true);
}

static inline void st7789_lcd_init(const uint8_t *init_seq) {
    const uint8_t *cmd = init_seq;
    while (*cmd) {
        st7789_lcd_write_cmd(cmd + 2, *cmd);
        sleep_ms(*(cmd + 1) * 5);
        cmd += *cmd + 2;
    }
}

static inline void st7789_lcd_set_cursor(uint16_t x, uint16_t y) {
    const uint8_t st7789_cursor_seq[] = {
            5, 0, ST7789_CASET, (uint8_t) (x >> 8), (uint8_t) (x & 0xff), DISPLAY_WIDTH >> 8, DISPLAY_WIDTH & 0xff,
            5, 0, ST7789_RASET, (uint8_t) (y >> 8), (uint8_t) (y & 0xff), DISPLAY_HEIGHT >> 8, DISPLAY_HEIGHT & 0xff,
            0
    };

    const uint8_t *cmd = st7789_cursor_seq;
    while (*cmd) {
        st7789_lcd_write_cmd(cmd + 2, *cmd);
        cmd += *cmd + 2;
    }

    // enable write
    st7789_lcd_write_cmd(ST7789_RAMWR);
    st7789_lcd_set_dc_cs(true, false);
}

static inline void st7789_lcd_set_rotation(uint8_t m) {
    /*
    static const uint8_t st7789_rotation_seq[] = {
            2, 0, ST7789_MADCTL, ST7789_MADCTL_RGB,
            0                                   // Terminate list
    };

    const uint8_t *cmd = st7789_rotation_seq;
    while (*cmd) {
        lcd_write_cmd(pio, sm, cmd + 2, *cmd);
        sleep_ms(*(cmd + 1) * 5);
        cmd += *cmd + 2;
    }
    */

    st7789_lcd_write_cmd(ST7789_MADCTL);
    switch (m) {
        case 0:
            st7789_lcd_put(m_pio, m_sm, ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
            break;
        case 1:
            st7789_lcd_put(m_pio, m_sm, ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            break;
        case 2:
            st7789_lcd_put(m_pio, m_sm, ST7789_MADCTL_RGB);
            break;
        case 3:
            st7789_lcd_put(m_pio, m_sm, ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
            break;
        default:
            break;
    }
}

void st7789_init() {
    uint offset = pio_add_program(m_pio, &st7789_lcd_program);
    st7789_lcd_program_init(m_pio, m_sm, offset, PIN_DIN, PIN_CLK, SERIAL_CLK_DIV);

    gpio_init(PIN_CS);
    gpio_init(PIN_DC);
    gpio_init(PIN_RESET);
    gpio_init(PIN_BL);
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_set_dir(PIN_DC, GPIO_OUT);
    gpio_set_dir(PIN_RESET, GPIO_OUT);
    gpio_set_dir(PIN_BL, GPIO_OUT);

    gpio_put(PIN_CS, true);
    gpio_put(PIN_RESET, true);
    st7789_lcd_init(st7789_init_seq);
    gpio_put(PIN_BL, true);
}

void st7789_start_pixels() {
    st7789_lcd_write_cmd(ST7789_RAMWR);
    st7789_lcd_set_dc_cs(true, false);
}

void st7789_put(uint16_t pixel) {
    st7789_lcd_put(m_pio, m_sm, pixel >> 8);
    st7789_lcd_put(m_pio, m_sm, pixel & 0xff);
}

void st7789_set_cursor(uint16_t x, uint16_t y) {
    st7789_lcd_set_cursor(x, y);
}
