//
// Created by cpasjuste on 30/03/23.
//

#include "platform.h"
#include "text.h"
#include "ui.h"

#include "fonts/future7pt7b.h"

using namespace mb;

Text::Text(int16_t x, int16_t y, const std::string &str, uint16_t color) : Widget() {
    // default stuff
    Ui::getDisplay()->setFont(&future7pt7b);
    Ui::getDisplay()->setTextWrap(false);

    Widget::setPosition(x, y);
    Widget::setSize(0, 0);
    m_color = color;
    setString(str);
}

Text::Text(int16_t x, int16_t y, int16_t w, int16_t h,
           const std::string &str, uint16_t color) : Widget() {
    // default stuff
    Ui::getDisplay()->setFont(&future7pt7b);
    Ui::getDisplay()->setTextWrap(false);

    Widget::setPosition(x, y);
    Widget::setSize(w, h);
    m_color = color;
    setString(str);
}

Text::Text(const Utility::Vec2i &pos, const Utility::Vec2i &size,
           const std::string &str, uint16_t color) : Widget() {
    // default stuff
    Ui::getDisplay()->setFont(&future7pt7b);
    Ui::getDisplay()->setTextWrap(false);

    Widget::setPosition(pos);
    Widget::setSize(size);
    m_color = color;
    setString(str);
}

std::string Text::getString() const {
    return m_text;
}

void Text::setString(const std::string &str) {
    m_text = str;
    Ui::getDisplay()->getTextBounds(m_text.c_str(), 0, 0,
                                    &m_bounds.x, &m_bounds.y,
                                    (uint16_t *) &m_bounds.w, (uint16_t *) &m_bounds.h);
    if (getSize().x == 0 || getSize().y == 0) {
        setSize(m_bounds.w, m_bounds.h);
    }
}

void Text::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;

    // now draw the text
    Ui::getDisplay()->setTextColor(m_color);
    Ui::getDisplay()->setClipArea({pos.x, pos.y, (int16_t) (pos.x + m_size.x), (int16_t) (pos.y + m_size.y)});
    Ui::getDisplay()->drawText((int16_t) (pos.x - m_bounds.x), (int16_t) (pos.y - m_bounds.y), m_text);
    Ui::getDisplay()->setClipArea({0, 0, Ui::getDisplay()->getSize().x, Ui::getDisplay()->getSize().y});

    // draw child's
    Widget::loop(pos, buttons);
}
