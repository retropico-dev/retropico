//
// Created by cpasjuste on 26/02/24.
//

#ifndef RETROPICO_OVERLAY_H
#define RETROPICO_OVERLAY_H

#include "platform.h"
#include "rectangle.h"
#include "retropico_config.h"
#include "widgets/volume.h"

namespace retropico {
    class Overlay : public p2d::Widget {
    public:
        explicit Overlay(Config *config, p2d::Platform *platform, const p2d::Utility::Vec4i &bounds);

        void onUpdate(p2d::Time delta) override;

    private:
        Config *p_config;
        p2d::Platform *p_platform;
        p2d::Clock m_visibility_clock;
        p2d::Clock m_input_clock;
        uint16_t m_buttons_old;

        VolumeWidget *p_volume_widget;
    };
}

#endif //RETROPICO_RETROPICO_OVERLAY_H
