//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

// Not enough ram to use "copy_to_ram" binary and double buffering.
// As a "speedup hack", we overwrite the surface in main thread while
// sending it to the display from core1 at the same time.
// this is not great but seems to do the job without major artifacts for now

//#define MB_DOUBLE_BUFFER 1

namespace mb {
    class PeanutGB : public Core {
    public:
        explicit PeanutGB(Platform *platform);

        ~PeanutGB() override;

        bool loadRom(const std::string &path) override;

        bool loadRom(Io::FileBuffer file) override;

        bool loop() override;

        Surface *getSurface(uint8_t idx) {
            return p_surface[idx];
        }

        [[nodiscard]] uint8_t getBufferIndex() const {
#if MB_DOUBLE_BUFFER
            return m_bufferIndex;
#else
            return 0;
#endif
        }

        void setBufferIndex(uint8_t idx) {
#if MB_DOUBLE_BUFFER
            m_bufferIndex = idx;
#else
            m_bufferIndex = 0;
#endif
        }

        [[nodiscard]] bool isScalingEnabled() const {
            return m_scaling;
        }

        void setScalingEnabled(bool enable) {
            m_scaling = enable;
        }

        [[nodiscard]] bool isFrameSkipEnabled() const {
            return m_frameSkip;
        }

        void setFrameSkipEnabled(bool enable) {
            m_frameSkip = enable;
        }

    private:
        Surface *p_surface[2] = {nullptr, nullptr};
        uint8_t m_bufferIndex = 0;
        bool m_scaling = false;
        bool m_frameSkip = false;
    };
}

#endif //MICROBOY_PEANUT_GB_H
