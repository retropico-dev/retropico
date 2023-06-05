//
// Created by cpasjuste on 05/06/23.
//

#include "display.h"

using namespace mb;

Display::Display(const Utility::Vec2i &displaySize, const Utility::Vec2i &surfaceSize) {
    m_size = displaySize;
    m_bpp = 2;
    m_pitch = m_size.x * m_bpp;
    p_surface = new Surface(surfaceSize);
}

void Display::drawPixel(const Utility::Vec2i &pos, uint16_t pixel) {
    if (!m_scale) {
        // draw to the display directly
        setPixel(pos, pixel);
    } else {
        // draw to the pixel buffer
        p_surface->setPixel(pos, pixel);
    }
}

void Display::flip() {
    // scale and write pixel buffer (surface) to display if needed
    if (m_scale) {
        auto srcSize = p_surface->getSize();
        auto dstSize = Utility::Vec2i(
                m_size.x, (int16_t) ((float) m_size.x * ((float) srcSize.y / (float) srcSize.x)));
        Utility::Vec2i dstPos = {0, (int16_t) ((m_size.y - dstSize.y) / 2)};
        if (dstSize.y > m_size.y) {
            dstSize = Utility::Vec2i((int16_t) ((float) m_size.y * ((float) srcSize.x / (float) srcSize.y)), m_size.y);
            dstPos = {(int16_t) ((m_size.x - dstSize.x) / 2), 0};
        }

        int x, y;
        int xRatio = (srcSize.x << 16) / dstSize.x;
        int yRatio = (srcSize.y << 16) / dstSize.y;
        for (int16_t i = 0; i < dstSize.y; i++) {
            for (int16_t j = 0; j < dstSize.x; j++) {
                if (j >= m_size.x || i >= m_size.y) {
                    continue;
                }
                x = (j * xRatio) >> 16;
                y = (i * yRatio) >> 16;
                setPixel({(int16_t) (j + dstPos.x), (int16_t) (i + dstPos.y)},
                         p_surface->getPixel({(int16_t) x, (int16_t) y}));
            }
        }
    }
}

Display::~Display() {
    delete (p_surface);
}
