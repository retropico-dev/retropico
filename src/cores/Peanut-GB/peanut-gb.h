//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_PEANUT_GB_H
#define MICROBOY_PEANUT_GB_H

#include "core.h"

namespace mb {
    class PeanutGB : public Core {
    public:
        explicit PeanutGB(Platform *platform);

        ~PeanutGB() override;

        bool loadRom(const std::string &path) override;

        bool loadRom(const uint8_t *buffer, size_t size) override;

        bool loop() override;

        Surface *getSurface(uint8_t idx) {
            return p_surface[idx];
        }

        [[nodiscard]] uint8_t getBufferIndex() const {
            return m_bufferIndex;
        }

        void setBufferIndex(uint8_t idx) {
            m_bufferIndex = idx;
        }

        [[nodiscard]] bool isBuffered() const {
            return m_doubleBuffer;
        }

        void setDoubleBuffering(bool enable) {
            m_doubleBuffer = enable;
        }

    private:
        Surface *p_surface[2] = {nullptr, nullptr};
        uint8_t m_bufferIndex = 0;
        bool m_doubleBuffer = false;
    };
}

#endif //MICROBOY_PEANUT_GB_H
