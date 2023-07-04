//
// Created by cpasjuste on 30/03/23.
//

#include "platform.h"
#include "ui.h"
#include "rectangle.h"

using namespace mb;

Rectangle::Rectangle(int16_t x, int16_t y, int16_t w, int16_t h,
                     bool fill, int16_t radius, uint16_t color) : Widget() {
    Widget::setPosition(x, y);
    Widget::setSize(w, h);
    m_color = color;
    m_fill = fill;
    m_radius = radius;
}

Rectangle::Rectangle(const Utility::Vec2i &pos, const Utility::Vec2i &size,
                     bool fill, int16_t radius, uint16_t color) : Widget() {
    Widget::setPosition(pos);
    Widget::setSize(size);
    m_color = color;
    m_fill = fill;
    m_radius = radius;
}

void Rectangle::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;

    if (m_fill) {
        if (m_radius > 0) {
            Ui::getDisplay()->fillRoundRect(pos.x, pos.y, m_size.x, m_size.y, m_radius, m_color);
        } else {
            Ui::getDisplay()->fillRect(pos.x, pos.y, m_size.x, m_size.y, m_color);
        }
    } else {
        if (m_radius > 0) {
            Ui::getDisplay()->drawRoundRect(pos.x, pos.y, m_size.x, m_size.y, m_radius, m_color);
        } else {
            Ui::getDisplay()->drawRect(pos.x, pos.y, m_size.x, m_size.y, m_color);
        }
    }

    Widget::loop(pos, buttons);
}
