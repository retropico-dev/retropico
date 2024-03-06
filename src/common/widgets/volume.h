//
// Created by cpasjuste on 26/02/24.
//

#ifndef RETROPICO_VOLUME_H
#define RETROPICO_VOLUME_H

#include "rectangle.h"
#include "text.h"

namespace retropico {
    class VolumeWidget : public p2d::Rectangle {
    public:
        explicit VolumeWidget(const p2d::Utility::Vec4i &bounds);

        // range: 0-100
        void setVolume(uint8_t volume);

    private:
        p2d::Rectangle *p_slider_rect;
        p2d::Text *p_text;
    };
}

#endif //RETROPICO_VOLUME_H
