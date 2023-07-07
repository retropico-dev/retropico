//
// Created by cpasjuste on 30/03/23.
//

#ifndef U_MICROBOY_BITMAP_H
#define U_MICROBOY_BITMAP_H

#include "widget.h"

namespace mb {
    class Bitmap : public Widget {
    public:
        typedef struct {
            const uint16_t *data;
            uint16_t width;
            uint16_t height;
            uint8_t dataSize;
        } Image;

        Bitmap(const Utility::Vec2i &pos, const Image *image);

        void loop(const Utility::Vec2i &pos, const uint16_t &buttons) override;

    private:
        const Image *m_image = nullptr;
    };
}

#endif //U_MICROBOY_BITMAP_H
