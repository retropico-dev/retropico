//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PICO_DISPLAY_H
#define MICROBOY_PICO_DISPLAY_H

namespace mb {
    class PicoDisplay : public Display {
    public:
        PicoDisplay();

        void setCursor(uint16_t x, uint16_t y) override;

        void setPixel(uint16_t color) override;

        void clear(uint16_t color = Black) override;

        void flip() override;

        uint8_t *getPixelBuffer(uint8_t index) {
            return p_pixelBuffer[index];
        }

    private:
        bool m_doubleBuffer = false;
        uint8_t *p_pixelBuffer[2];
        uint8_t m_bufferIndex = 0;
        Utility::Vec2i m_cursor;
    };
}

#endif //MICROBOY_PICO_DISPLAY_H
