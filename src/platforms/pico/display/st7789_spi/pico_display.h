//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PICO_DISPLAY_H
#define MICROBOY_PICO_DISPLAY_H

namespace mb {
    class PicoDisplay : public Display {
    public:
        PicoDisplay();

        void clear() override;

        void flip() override;

        void drawPixel(int16_t x, int16_t y, uint16_t color) override;

    private:
    };
}

#endif //MICROBOY_PICO_DISPLAY_H
