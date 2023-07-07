//
// Created by cpasjuste on 30/03/23.
//

#include "platform.h"
#include "ui.h"
#include "rectangle.h"

using namespace mb;

Rectangle::Rectangle(int16_t x, int16_t y, int16_t w, int16_t h,
                     uint16_t color, int16_t radius) : Widget() {
    Widget::setPosition(x, y);
    Widget::setSize(w, h);
    m_color = color;
    m_radius = radius;
}

Rectangle::Rectangle(const Utility::Vec2i &pos, const Utility::Vec2i &size,
                     uint16_t color, int16_t radius) : Widget() {
    Widget::setPosition(pos);
    Widget::setSize(size);
    m_color = color;
    m_radius = radius;
}

Rectangle::Rectangle(const Utility::Vec4i &bounds, uint16_t color, int16_t radius) {
    Widget::setPosition({bounds.x, bounds.y});
    Widget::setSize({bounds.w, bounds.h});
    m_color = color;
    m_radius = radius;
}

void Rectangle::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;

    // first draw outline if needed
    if (m_outline_thickness > 0 && m_outline_color != Ui::Color::Transparent) {
        for (uint16_t i = 1; i < m_outline_thickness + 1; i++) {
            if (m_radius > 0) {
                Ui::getDisplay()->drawRoundRect((int16_t) (pos.x - (1 * i)), (int16_t) (pos.y - (1 * i)),
                                                (int16_t) (m_size.x + (2 * i)), (int16_t) (m_size.y + (2 * i)),
                                                m_radius, m_outline_color);
            } else {
                Ui::getDisplay()->drawRect((int16_t) (pos.x - (1 * i)), (int16_t) (pos.y - (1 * i)),
                                           (int16_t) (m_size.x + (2 * i)), (int16_t) (m_size.y + (2 * i)),
                                           m_outline_color);
            }
        }
    }

    // now fill rectangle if needed
    if (m_color != Ui::Color::Transparent) {
        if (m_radius > 0) {
            Ui::getDisplay()->fillRoundRect(pos.x, pos.y, m_size.x, m_size.y, m_radius, m_color);
        } else {
            Ui::getDisplay()->fillRect(pos.x, pos.y, m_size.x, m_size.y, m_color);
        }
    }

    // draw child's
    Widget::loop(pos, buttons);
}
