//
// Created by cpasjuste on 05/06/23.
//

#include <cstdio>
#include "display.h"

using namespace mb;

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef _swap_int16_t
#define _swap_int16_t(a, b)                                                    \
  {                                                                            \
    int16_t t = a;                                                             \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

Display::Display(const Utility::Vec2i &size) {
    m_size = size;
    m_bpp = 2;
    m_pitch = m_size.x * m_bpp;
}

void Display::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint16_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        _swap_int16_t(x0, y0);
        _swap_int16_t(x1, y1);
    }

    if (x0 > x1) {
        _swap_int16_t(x0, x1);
        _swap_int16_t(y0, y1);
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);
    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}

void Display::drawSurface(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size) {
    if (!surface) return;

    if (size == surface->getSize()) {
        for (uint16_t y = 0; y < size.y; y++) {
            for (uint16_t x = 0; x < size.x; x++) {
                drawPixel(x + pos.x, y + pos.y, surface->getPixel(x, y));
            }
        }
    } else {
        auto srcSize = surface->getSize();
        auto dstSize = Utility::Vec2i(
                size.x, (int16_t) ((float) size.x * ((float) srcSize.y / (float) srcSize.x)));
        Utility::Vec2i dstPos = {pos.x, (int16_t) (pos.y + (size.y - dstSize.y) / 2)};
        if (dstSize.y > size.y) {
            dstSize = Utility::Vec2i((int16_t) ((float) size.y * ((float) srcSize.x / (float) srcSize.y)), size.y);
            dstPos = {(int16_t) (pos.x + (size.x - dstSize.x) / 2), pos.y};
        }

        int x, y;
        int xRatio = (srcSize.x << 16) / dstSize.x;
        int yRatio = (srcSize.y << 16) / dstSize.y;
        for (int16_t i = 0; i < dstSize.y; i++) {
            for (int16_t j = 0; j < dstSize.x; j++) {
                if (j >= size.x || i >= size.y) {
                    continue;
                }
                x = (j * xRatio) >> 16;
                y = (i * yRatio) >> 16;
                drawPixel(j + dstPos.x, i + dstPos.y, surface->getPixel(x, y));
            }
        }
    }
}
