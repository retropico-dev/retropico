//
// Created by cpasjuste on 02/08/23.
//

#ifndef MICROBOY_INFOBOX_H
#define MICROBOY_INFOBOX_H

#include "rectangle.h"
#include "text.h"
#include "utility.h"

namespace mb {
    class InfoBox : public p2d::Rectangle {
    public:
        InfoBox(const p2d::Utility::Vec2i &pos, const p2d::Utility::Vec2i &size);

        void show(const std::string &text, uint32_t m_millis = 0);

        void hide();

    private:
        void loop(const p2d::Utility::Vec2i &pos, const uint16_t &buttons) override;

        p2d::Text *p_text{};
        p2d::Clock m_clock{};
        uint32_t m_millis = 0;
    };
}

#endif //MICROBOY_INFOBOX_H