//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_DISPLAY_H
#define MICROBOY_DISPLAY_H

#include "surface.h"
#include "utility.h"

namespace mb {
    class Display {
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
        virtual ~Display() = default;

        // clear the display (hardware dependant, to be implemented)
        virtual void clear() {}

        // flip the display (hardware dependant, to be implemented)
        virtual void flip() {}

        // draw a pixel to the display (hardware dependant, to be implemented)
        virtual void drawPixel(uint16_t x, uint16_t y, uint16_t color) {}

        void drawPixel(const Utility::Vec2i &pos, uint16_t color) {
            drawPixel(pos.x, pos.y, color);
        }

        void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color);

        // draw a surface (pixel buffer) to the display with scaling if requested
        void drawSurface(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size);

        void drawSurface(Surface *surface, const Utility::Vec2i &pos) {
            if (!surface) return;
            drawSurface(surface, pos, surface->getSize());
        }

        Utility::Vec2i getSize() { return m_size; };

    protected:
        Utility::Vec2i m_size{};
        int m_pitch = 0;
        int m_bpp = 2;
    };
}

#endif //MICROBOY_DISPLAY_H
