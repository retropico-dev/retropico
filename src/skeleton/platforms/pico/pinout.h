//
// Created by cpasjuste on 14/06/23.
//

#ifndef MICROBOY_PINOUT_H
#define MICROBOY_PINOUT_H

// SPI LCD PINS         GPIO                        FEATHER RP2040
#define LCD_PIN_DC      8   // SPI1 RX              6
#define LCD_PIN_CS      9   // SPI1 CSn             9
#define LCD_PIN_CLK     10  // SPI1 SCK             10
#define LCD_PIN_DIN     11  // SPI1 TX (MOSI)       11
#define LCD_PIN_BL      12  // GPI0                 12
#define LCD_PIN_RESET   13  // GPIO                 13

// SPI SDCARD PINS
#define SD_PIN_CS       6   // SPI0 CSn             D4
#define SD_PIN_CLK      18  // SPI0 SCK             SCK
#define SD_PIN_MOSI     19  // SPI0 TX              MO
#define SD_PIN_MISO     20  // SPI0 RX              MI

// AUDIO PINS (I2S, DIGITAL)
#define AUDIO_PIN_DATA  26  // DIN                  A0
#define AUDIO_PIN_CLOCK 27  // BCLK                 A1
#define AUDIO_PIN_LRC   28  // LRC (CLOCK + 1)      A2

// BUTTONS PINS
#if !defined(NDEBUG) && defined(PICO_STDIO_UART)
#define BTN_PIN_A       (-1)    //                  25
#define BTN_PIN_B       (-1)    //                  24
#define BTN_PIN_START   (-1)    //                  RX
#define BTN_PIN_SELECT  (-1)    //                  TX
#define BTN_PIN_LEFT    (-1)    //                  SCL
#define BTN_PIN_RIGHT   (-1)    //                  A3
#define BTN_PIN_UP      (-1)    //                  5
#define BTN_PIN_DOWN    (-1)    //                  SDA
#else
#define BTN_PIN_A       (25)    //                  25
#define BTN_PIN_B       (24)    //                  24
#define BTN_PIN_START   (1)     //                  RX
#define BTN_PIN_SELECT  (0)     //                  TX
#define BTN_PIN_LEFT    (3)     //                  SCL
#define BTN_PIN_RIGHT   (29)    //                  A3
#define BTN_PIN_UP      (7)     //                  5
#define BTN_PIN_DOWN    (2)     //                  SDA
#endif

#endif //MICROBOY_PINOUT_H
