//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PICO_DISPLAY_BUFFERED_H
#define MICROBOY_PICO_DISPLAY_BUFFERED_H

namespace mb {
    class PicoDisplayBuffered : public Display {
    public:
        PicoDisplayBuffered();

        void setCursor(uint16_t x, uint16_t y) override;

        void setPixel(uint16_t color) override;

        void clear(uint16_t color = Black) override;

        void flip() override;

        uint8_t *getPixelBuffer(uint8_t index) {
            return p_pixelBuffer[index];
        }

    private:
        Utility::Vec2i m_cursor;
        uint8_t *p_pixelBuffer[2]{};
        uint8_t m_bufferIndex = 0;
    };
}

#endif //MICROBOY_PICO_DISPLAY_BUFFERED_H
