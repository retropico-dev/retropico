//
// Created by cpasjuste on 05/06/23.
//

#include "display.h"

using namespace mb;

Display::Display(const Utility::Vec2i &size) : Adafruit_GFX(size.x, size.y) {
    m_size = size;
    m_bpp = 2;
    m_pitch = m_size.x * m_bpp;
}

void Display::drawSurface(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size) {
    if (!surface) return;

    if (size == surface->getSize()) {
        for (uint16_t y = 0; y < size.y; y++) {
            for (uint16_t x = 0; x < size.x; x++) {
                drawPixel((int16_t) (x + pos.x), (int16_t) (y + pos.y), surface->getPixel(x, y));
            }
        }
    } else {
        int x, y;
        auto pitch = surface->getPitch();
        auto bpp = surface->getBpp();
        auto pixels = surface->getPixels();
        auto srcSize = surface->getSize();
        int xRatio = (srcSize.x << 16) / size.x;
        int yRatio = (srcSize.y << 16) / size.y;
        for (int16_t i = 0; i < size.y; i++) {
            for (int16_t j = 0; j < size.x; j++) {
                if (j >= size.x || i >= size.y) {
                    continue;
                }
                x = (j * xRatio) >> 16;
                y = (i * yRatio) >> 16;
                // big fps drop in surface->getPixel ?!
                //drawPixel((int16_t) (j + pos.x), (int16_t) (i + pos.y), surface->getPixel(x, y));
                uint16_t p = *(uint16_t *) (pixels + y * pitch + x * bpp);
                drawPixel((int16_t) (j + pos.x), (int16_t) (i + pos.y), p);
            }
        }
    }
}
