//
// Created by cpasjuste on 30/03/23.
//

#include <algorithm>
#include "platform.h"
#include "widget.h"

using namespace mb;

void Widget::add(Widget *widget) {
    if (widget) {
        widget->p_parent = this;
        p_child_list.push_back(widget);
    }
}

void Widget::remove(Widget *widget) {
    if (!p_child_list.empty()) {
        p_child_list.erase(std::remove(
                p_child_list.begin(), p_child_list.end(), widget), p_child_list.end());
    }
}

void Widget::setPosition(int16_t x, int16_t y) {
    m_position.x = x;
    m_position.y = y;
}

void Widget::setPosition(const Utility::Vec2i &pos) {
    setPosition(pos.x, pos.y);
}

Utility::Vec2i Widget::getPosition() {
    return m_position;
}

void Widget::setSize(int16_t x, int16_t y) {
    m_size.x = x;
    m_size.y = y;
}

void Widget::setSize(const Utility::Vec2i &size) {
    m_size.x = size.x;
    m_size.y = size.y;
}

Utility::Vec4i Widget::getBounds() {
    return {m_position.x, m_position.y, m_size.x, m_size.y};
}

void Widget::loop(const Utility::Vec2i &pos) {
    if (!isVisible()) return;

    for (auto &widget: p_child_list) {
        if (widget && widget->isVisible()) {
            Utility::Vec2i v = {(int16_t) (pos.x + widget->m_position.x),
                                (int16_t) (pos.y + widget->m_position.y)};
            switch (widget->m_origin) {
                case Origin::Left:
                    v.y -= widget->m_size.y / 2;
                    break;
                case Origin::Top:
                    v.x -= widget->m_size.x / 2;
                    break;
                case Origin::TopRight:
                    v.x -= widget->m_size.x;
                    break;
                case Origin::Right:
                    v.x -= widget->m_size.x;
                    v.y -= widget->m_size.y / 2;
                    break;
                case Origin::BottomRight:
                    v.x -= widget->m_size.x;
                    v.y -= widget->m_size.y;
                    break;
                case Origin::Bottom:
                    v.x -= widget->m_size.x / 2;
                    v.y -= widget->m_size.y;
                    break;
                case Origin::BottomLeft:
                    v.y -= widget->m_size.y;
                    break;
                case Origin::Center:
                    v.x -= widget->m_size.x / 2;
                    v.y -= widget->m_size.y / 2;
                    break;
                case Origin::TopLeft:
                default:
                    break;
            }

            widget->loop(v);
        }
    }
}

Widget::~Widget() {
    for (auto widget = p_child_list.begin(); widget != p_child_list.end();) {
        if (*widget) {
            delete (*widget);
        }
    }
    p_child_list.clear();
    if (p_parent) {
        p_parent->remove(this);
    }
}
