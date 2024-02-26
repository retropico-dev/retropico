//
// Created by cpasjuste on 06/06/23.
//

#ifndef RETROPICO_PEANUT_GB_H
#define RETROPICO_PEANUT_GB_H

#include "core.h"

namespace retropico {
    class PeanutGB : public Core {
    public:
        explicit PeanutGB(const p2d::Display::Settings &displaySettings);

        bool loop() override;

        //void close() override;

        bool loadRom(const p2d::Io::File &file) override;

        [[nodiscard]] bool isFrameSkipEnabled() const {
            return m_frameSkip;
        }

        void setFrameSkipEnabled(bool enable) {
            m_frameSkip = enable;
        }

#if 0   // we don't want unscaled rendering, too small...
        [[nodiscard]] bool isScalingEnabled() const {
            return m_scaling;
        }

        void setScalingEnabled(bool enable) {
            if (enable == m_scaling) return;
            if (!enable) {
                getDisplay()->clear();
            }
            m_scaling = enable;
        }
#endif

    private:
        bool m_scaling = true;
        bool m_frameSkip = false;
    };
}

#endif //RETROPICO_PEANUT_GB_H
