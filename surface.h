//
// Created by cpasjuste on 05/06/23.
//

#ifndef MICROBOY_SURFACE_H
#define MICROBOY_SURFACE_H

#include <cstdlib>
#include "utility.h"

namespace mb {
    class Surface {
    public:
        explicit Surface(const Utility::Vec2i &size) {
            m_size = size;
            p_buffer = (uint8_t *) malloc(m_size.x * m_size.y * m_bpp);
            m_pitch = m_size.x * m_bpp;
        }

        ~Surface() {
            if (p_buffer) free(p_buffer);
        }

        void setPixel(const Utility::Vec2i &pos, uint16_t pixel) {
            if (!p_buffer) return;
            if (pos.x > m_size.x || pos.y > m_size.y) return;
            *(uint16_t *) (p_buffer + pos.y * m_pitch + pos.x * m_bpp) = pixel;
        }

        uint16_t getPixel(const Utility::Vec2i &pos) {
            if (!p_buffer) return 0;
            if (pos.x > m_size.x || pos.y > m_size.y) return 0;
            return *(uint16_t *) (p_buffer + pos.y * m_pitch + pos.x * m_bpp);
        }

        uint8_t *getPixels() { return p_buffer; };

        Utility::Vec2i getSize() { return m_size; };

        [[nodiscard]] uint16_t getPitch() const { return m_pitch; };

        [[nodiscard]] uint8_t getBpp() const { return m_bpp; };

    private:
        uint8_t *p_buffer = nullptr;
        Utility::Vec2i m_size;
        uint16_t m_pitch = 0;
        uint8_t m_bpp = 2;
    };
}

#endif //MICROBOY_SURFACE_H
