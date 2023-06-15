//
// Created by cpasjuste on 14/06/23.
//

#ifndef MICROBOY_PINOUT_H
#define MICROBOY_PINOUT_H

// SPI LCD PINS
#define LCD_PIN_DC      8   // SPI1 RX
#define LCD_PIN_CS      9   // SPI1 CSn
#define LCD_PIN_CLK     10  // SPI1 SCK
#define LCD_PIN_DIN     11  // SPI1 TX (MOSI)
#define LCD_PIN_BL      12  // GPI0
#define LCD_PIN_RESET   13  // GPIO

// SPI SDCARD PINS
#define SD_PIN_CS       6   // SPI0 CSn
#define SD_PIN_CLK      18  // SPI0 SCK
#define SD_PIN_MOSI     19  // SPI0 TX
#define SD_PIN_MISO     20  // SPI0 RX

// AUDIO PINS (I2S, DIGITAL)
#define AUDIO_PIN_DATA  26  // DIN
#define AUDIO_PIN_CLOCK 27  // BCLK
#define AUDIO_PIN_LRC   28  // LRC (MUST BE AUDIO_PIN_CLOCK + 1)

// BUTTONS PINS
#define BTN_PIN_B1      (-1)
#define BTN_PIN_B2      (-1)
#define BTN_PIN_START   (-1)
#define BTN_PIN_SELECT  (-1)
#define BTN_PIN_LEFT    (-1)
#define BTN_PIN_RIGHT   (-1)
#define BTN_PIN_UP      (-1)
#define BTN_PIN_DOWN    (-1)

#endif //MICROBOY_PINOUT_H
