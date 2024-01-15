//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

#define MB_DOUBLE_BUFFER 1

namespace mb {
    class PeanutGB : public Core {
    public:
        explicit PeanutGB(p2d::Platform *platform);

        bool loadRom(p2d::Io::FileBuffer file) override;

        bool loop(uint16_t buttons) override;

        [[nodiscard]] bool isScalingEnabled() const {
            return m_scaling;
        }

        void setScalingEnabled(bool enable) {
            if (enable == m_scaling) return;
            if (!enable) {
                p_platform->getDisplay()->clear();
            }
            m_scaling = enable;
        }

        [[nodiscard]] bool isFrameSkipEnabled() const {
            return m_frameSkip;
        }

        void setFrameSkipEnabled(bool enable) {
            m_frameSkip = enable;
        }

    private:
        bool m_scaling = true;
        bool m_frameSkip = false;
    };
}

#endif //MICROBOY_PEANUT_GB_H
