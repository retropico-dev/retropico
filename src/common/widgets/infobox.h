//
// Created by cpasjuste on 02/08/23.
//

#ifndef RETROPICO_INFOBOX_H
#define RETROPICO_INFOBOX_H

#include <platform.h>
#include "text.h"
#include "utility.h"

namespace retropico {
    class InfoBox final : public p2d::Rectangle {
    public:
        InfoBox(const p2d::Utility::Vec2i &pos, const p2d::Utility::Vec2i &size,
                const uint16_t &color, const uint16_t &bg_color);

        void show(const std::string &text, uint32_t m_millis = 0, p2d::Platform *platform = nullptr);

        void hide();

    private:
        void onUpdate(p2d::Time delta) override;

        p2d::Text *p_text{};
        p2d::Clock m_clock{};
        uint32_t m_millis = 0;
    };
}

#endif //RETROPICO_INFOBOX_H
