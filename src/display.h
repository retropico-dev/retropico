//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_H
#define MICROBOY_DISPLAY_H

#include "Adafruit_GFX.h"
#include "surface.h"
#include "utility.h"

namespace mb {
    class Display : public Adafruit_GFX {
    public:
        enum Color {
            Black = 0x0000,
            Blue = 0x001F,
            Red = 0xF800,
            Green = 0x07E0,
            Cyan = 0x07FF,
            Magenta = 0xF81F,
            Yellow = 0xFFE0,
            White = 0xFFFF,
        };

        // init a display (hardware dependant, to be implemented)
        // default display size used for "ST7789 1.54" TFT IPS 240x240"
        explicit Display(const Utility::Vec2i &size = {240, 240});

        // destroy the display (hardware dependant, to be implemented)
        ~Display() override = default;

        // clear the display (hardware dependant, to be implemented)
        virtual void clear() {}

        // flip the display (hardware dependant, to be implemented)
        virtual void flip() {}

        // draw a pixel to the display (hardware dependant, to be implemented)
        void drawPixel(int16_t x, int16_t y, uint16_t color) override {};

        void drawPixel(const Utility::Vec2i &pos, uint16_t color) {
            drawPixel(pos.x, pos.y, color);
        }

        // draw a surface (pixel buffer) to the display with scaling if requested
        void drawSurface(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size);

        void drawSurface(Surface *surface, const Utility::Vec2i &pos) {
            if (!surface) return;
            drawSurface(surface, pos, surface->getSize());
        }

        void drawSurface(Surface *surface) {
            if (!surface) return;
            drawSurface(surface, {0, 0}, surface->getSize());
        }

        Utility::Vec2i getSize() { return m_size; };

    protected:
        Utility::Vec2i m_size{};
        int m_pitch = 0;
        int m_bpp = 2;
    };
}

#endif //MICROBOY_DISPLAY_H