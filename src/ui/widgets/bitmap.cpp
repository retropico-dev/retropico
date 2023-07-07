//
// Created by cpasjuste on 30/03/23.
//

#include "platform.h"
#include "ui.h"
#include "bitmap.h"

using namespace mb;

Bitmap::Bitmap(const Utility::Vec2i &pos, const Image *image) : Widget() {
    m_image = image;
    Widget::setPosition(pos);
    Widget::setSize((int16_t) m_image->width, (int16_t) m_image->height);
}

void Bitmap::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;

    Ui::getDisplay()->drawRGBBitmap(pos.x, pos.y, m_image->data,
                                    (int16_t) m_image->width, (int16_t) m_image->height);

    // draw child's
    Widget::loop(pos, buttons);
}
