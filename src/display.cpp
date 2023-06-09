//
// Created by cpasjuste on 05/06/23.
//

#include "display.h"

using namespace mb;

Display::Display(const Utility::Vec2i &size) : Adafruit_GFX(size.x, size.y) {
    m_size = size;
    m_bpp = 2;
    m_pitch = m_size.x * m_bpp;

    // pixel line buffer for drawSurface
    m_line_buffer = (uint16_t *) malloc(m_pitch);
    memset(m_line_buffer, 0, m_pitch);
}

void Display::drawSurface(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size) {
    if (!surface) return;

    if (size == surface->getSize()) {
        auto pixels = surface->getPixels();
        auto pitch = surface->getPitch();
        auto width = surface->getSize().x;
        for (uint8_t y = 0; y < size.y; y++) {
            drawPixelLine(pos.x, pos.y + y, width, (uint16_t *) (pixels + y * pitch));
        }
    } else {
        // nearest-neighbor scaling
        int x, y;
        auto pitch = surface->getPitch();
        auto bpp = surface->getBpp();
        auto pixels = surface->getPixels();
        auto srcSize = surface->getSize();
        int xRatio = (srcSize.x << 16) / size.x;
        int yRatio = (srcSize.y << 16) / size.y;
        for (uint8_t i = 0; i < size.y; i++) {
            for (uint8_t j = 0; j < size.x; j++) {
                //if (j >= m_size.x || i >= m_size.y) {
                //    continue;
                //}
                x = (j * xRatio) >> 16;
                y = (i * yRatio) >> 16;
                m_line_buffer[j + pos.x] = *(uint16_t *) (pixels + y * pitch + x * bpp);
            }
            drawPixelLine(pos.x, i + pos.y, size.x, m_line_buffer);
        }
    }
}
