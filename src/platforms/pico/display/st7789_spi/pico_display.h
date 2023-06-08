//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PICO_DISPLAY_H
#define MICROBOY_PICO_DISPLAY_H

#define PIN_DIN 19      // SPI0 TX  (MOSI)
#define PIN_CLK 18      // SPI0 SCK
#define PIN_CS 17       // SPI0 CSn
#define PIN_DC 16       // SPI0 RX
#define PIN_RESET 21    // GPIO
#define PIN_BL 20       // GPIO

namespace mb {
    class PicoDisplay : public Display {
    public:
        PicoDisplay();

        void clear() override;

        void flip() override;

        void drawPixel(int16_t x, int16_t y, uint16_t color) override;

        //void drawPixelLine(int16_t y, uint8_t *pixels, size_t len) override;

        //void drawSurface(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size) override;

    private:
        uint8_t *p_pixelBuffer = nullptr;
    };
}

#endif //MICROBOY_PICO_DISPLAY_H
