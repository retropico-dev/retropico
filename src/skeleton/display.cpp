//
// Created by cpasjuste on 05/06/23.
//

#include "platform.h"

using namespace mb;

Display::Display(const Utility::Vec2i &size) : Adafruit_GFX(size.x, size.y) {
    m_clip = {0, 0, size.x, size.y};
    m_size = size;
    m_bpp = 2;
    m_pitch = m_size.x * m_bpp;

    // pixel line buffer for drawSurface
    m_line_buffer = (uint16_t *) malloc(m_pitch);
    memset(m_line_buffer, 0, m_pitch);
}

// very slow, obviously...
void in_ram(Display::drawPixel)(int16_t x, int16_t y, uint16_t color) {
    if ((x < m_clip.x) || (y < m_clip.y)
        || x >= m_clip.x + m_clip.w || y >= m_clip.y + m_clip.h)
        return;

    if (rotation) {
        int16_t t;
        switch (rotation) {
            case 1:
                t = x;
                x = (int16_t) (m_size.x - 1 - y);
                y = t;
                break;
            case 2:
                x = (int16_t) (m_size.x - 1 - x);
                y = (int16_t) (m_size.y - 1 - y);
                break;
            case 3:
                t = x;
                x = y;
                y = (int16_t) (m_size.y - 1 - t);
                break;
            default:
                break;
        }
    }

    setCursor(x, y);
    setPixel(color);
}

// faster
void in_ram(Display::drawPixelLine)(const uint16_t *pixels, uint16_t width, const Format &format) {
    if (format == Format::RGB565) {
        for (uint_fast16_t i = 0; i < width; i++) {
            setPixel(pixels[i]);
        }
    } else {
        uint_fast16_t p;
        uint_fast8_t red, green, blue;
        for (uint_fast16_t i = 0; i < width; i++) {
            p = pixels[i];
            red = (p >> 8) & 0xF;
            green = (p >> 4) & 0xF;
            blue = p & 0xF;
            red = (red << 1) | (red >> 3);
            green = (green << 2) | (green >> 2);
            blue = (blue << 1) | (blue >> 3);
            setPixel((red << 11) | (green << 5) | blue);
        }
    }
}

void in_ram(Display::drawSurface)(Surface *surface, const Utility::Vec2i &pos, const Utility::Vec2i &size) {
    if (!surface) return;

    if (size == surface->getSize()) {
        auto pixels = surface->getPixels();
        auto pitch = surface->getPitch();
        auto width = surface->getSize().x;
        for (uint8_t y = 0; y < size.y; y++) {
            setCursor(pos.x, pos.y + y);
            drawPixelLine((uint16_t *) (pixels + y * pitch), width);
        }
    } else {
#if 1
        // nearest-neighbor scaling
        int x, y;
        auto pitch = surface->getPitch();
        auto bpp = surface->getBpp();
        auto pixels = surface->getPixels();
        auto srcSize = surface->getSize();
        int xRatio = (srcSize.x << 16) / size.x + 1;
        int yRatio = (srcSize.y << 16) / size.y + 1;

        setCursor(pos.x, pos.y);

        for (uint8_t i = 0; i < size.y; i++) {
            for (uint8_t j = 0; j < size.x; j++) {
                x = (j * xRatio) >> 16;
                y = (i * yRatio) >> 16;
                m_line_buffer[j + pos.x] = *(uint16_t *) (pixels + y * pitch + x * bpp);
            }
            if (size.x == m_size.x) {
                drawPixelLine(m_line_buffer, size.x);
            } else {
                setCursor(pos.x, i + pos.y);
                drawPixelLine(m_line_buffer, size.x);
            }
        }
#else
        // bi-linear interpolation
        auto pixels = (uint16_t *) surface->getPixels();
        auto srcSize = surface->getSize();
        int x_ratio = (srcSize.x << 16) / size.x + 1;
        int y_ratio = (srcSize.y << 16) / size.y + 1;
        int x, y, x_diff, y_diff;
        uint16_t a, b, c, d;

        setCursor(pos.x, pos.y);

        for (int i = 0; i < size.y; i++) {
            y = (i * y_ratio) >> 16;
            y_diff = ((i * y_ratio) >> 8) & 0xFF;

            for (int j = 0; j < size.x; j++) {
                x = (j * x_ratio) >> 16;
                x_diff = ((j * x_ratio) >> 8) & 0xFF;

                a = pixels[(y * srcSize.x) + x];
                b = pixels[(y * srcSize.x) + x + 1];
                c = pixels[((y + 1) * srcSize.x) + x];
                d = pixels[((y + 1) * srcSize.x) + x + 1];

                uint16_t red = (((a & 0xF800) >> 11) * (256 - x_diff) * (256 - y_diff) +
                                ((b & 0xF800) >> 11) * x_diff * (256 - y_diff) +
                                ((c & 0xF800) >> 11) * y_diff * (256 - x_diff) +
                                ((d & 0xF800) >> 11) * x_diff * y_diff) >> 16;

                uint16_t green = (((a & 0x07E0) >> 5) * (256 - x_diff) * (256 - y_diff) +
                                  ((b & 0x07E0) >> 5) * x_diff * (256 - y_diff) +
                                  ((c & 0x07E0) >> 5) * y_diff * (256 - x_diff) +
                                  ((d & 0x07E0) >> 5) * x_diff * y_diff) >> 16;

                uint16_t blue = ((a & 0x001F) * (256 - x_diff) * (256 - y_diff) +
                                 (b & 0x001F) * x_diff * (256 - y_diff) +
                                 (c & 0x001F) * y_diff * (256 - x_diff) +
                                 (d & 0x001F) * x_diff * y_diff) >> 16;

                //drawPixel(j, i, pixel);
                m_line_buffer[j + pos.x] = (red << 11) | (green << 5) | blue;
            }
            if (size.x == m_size.x) {
                drawPixelLine(m_line_buffer, size.x);
            } else {
                setCursor(pos.x, i + pos.y);
                drawPixelLine(m_line_buffer, size.x);
            }
        }
#endif
    }
}

void in_ram(Display::clear)(uint16_t color) {
    setCursor(0, 0);
    for (int y = 0; y < m_size.y; y++) {
        for (int x = 0; x < m_size.x; x++) {
            setPixel(color);
        }
    }
}

Display::~Display() {
    free(m_line_buffer);
}
