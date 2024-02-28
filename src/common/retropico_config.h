//
// Created by cpasjuste on 26/02/24.
//

#ifndef RETROPICO_CONFIG_H
#define RETROPICO_CONFIG_H

#include <cstdint>
#include "io.h"

#define LIST_BUFFER_MAX 6

namespace retropico {
    class Config {
    public:
        Config();

        bool save();

        uint8_t getVolume() { return m_data.volume; }

        void setVolume(uint8_t volume) { m_data.volume = volume; }

        uint8_t getBrightness() { return m_data.brightness; }

        void setBrightness(uint8_t brightness) { m_data.brightness = brightness; }

        p2d::Io::ListBuffer *getListBuffer(uint8_t index) {
            return &m_data.listBuffers[index];
        }

    private:
        struct Data {
            uint8_t volume = 50;
            uint8_t brightness = 100;
            p2d::Io::ListBuffer listBuffers[LIST_BUFFER_MAX];
        };

        Data m_data{};
    };
}

#endif //RETROPICO_CONFIG_H
