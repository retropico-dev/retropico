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
        // init a display (hardware dependant, to be implemented)
        // default display size used for "ST7789 1.54" TFT IPS 240x240"
        explicit Display(const Utility::Vec2i &displaySize = {240, 240},
                         const Utility::Vec2i &surfaceSize = {160, 144});

        // destroy the display (hardware dependant, to be implemented)
        virtual ~Display();

        // clear the display (hardware dependant, to be implemented)
        virtual void clear() {};

        // flip the display (hardware dependant, to be implemented)
        virtual void flip();

        // draw a pixel to the pixel buffer
        void drawPixel(const Utility::Vec2i &pos, uint16_t pixel);

        Utility::Vec2i getSize() { return m_size; };

        void setScaled(bool scaled) { m_scale = scaled; };

        [[nodiscard]] bool isScaled() const { return m_scale; };

    protected:
        Surface *p_surface = nullptr;
        Utility::Vec2i m_size{};
        int m_pitch = 0;
        int m_bpp = 2;
        bool m_scale = false;

        // draw a pixel to the display (hardware dependant)
        virtual void setPixel(const Utility::Vec2i &pos, uint16_t pixel) {};
    };
}

#endif //MICROBOY_DISPLAY_H
