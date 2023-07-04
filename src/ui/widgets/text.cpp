//
// Created by cpasjuste on 30/03/23.
//

#include "platform.h"
#include "text.h"
#include "ui.h"

//#include "fonts/future6pt7b.h"
#include "fonts/future7pt7b.h"
//#include "fonts/future8pt7b.h"

using namespace mb;

Text::Text(int16_t x, int16_t y, int16_t w, int16_t h,
           const std::string &str, uint16_t color) : Widget() {
    // default stuff
    Ui::getPlatform()->getDisplay()->setFont(&future7pt7b);
    Ui::getPlatform()->getDisplay()->setTextWrap(false);

    Widget::setPosition(x, y);
    Widget::setSize(w, h);
    m_color = color;
    setString(str);
}

Text::Text(const Utility::Vec2i &pos, const Utility::Vec2i &size,
           const std::string &str, uint16_t color) : Widget() {
    // default stuff
    Ui::getPlatform()->getDisplay()->setFont(&future7pt7b);
    Ui::getPlatform()->getDisplay()->setTextWrap(false);

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

    //
    Ui::getDisplay()->getTextBounds(m_text.c_str(), 0, 0,
                                    &m_bounds.x, &m_bounds.y,
                                    (uint16_t *) &m_bounds.w, (uint16_t *) &m_bounds.h);
    // TODO: handle recursive loop
    while (m_bounds.w > getSize().x) {
        m_text.erase(m_text.size() - 1, 1);
        Ui::getDisplay()->getTextBounds(m_text.c_str(), 0, 0,
                                        &m_bounds.x, &m_bounds.y,
                                        (uint16_t *) &m_bounds.w, (uint16_t *) &m_bounds.h);

    }
}

void Text::loop(const Utility::Vec2i &pos, const uint16_t &buttons) {
    if (!isVisible()) return;

    // now draw the text
    Ui::getDisplay()->setTextColor(m_color);
    Ui::getDisplay()->drawText((int16_t) (pos.x - m_bounds.x), (int16_t) (pos.y - m_bounds.y), m_text);

    // draw child's
    Widget::loop(pos, buttons);
}
