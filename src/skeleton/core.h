//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_CORE_H
#define MICROBOY_CORE_H

#include <string>
#include "platform.h"

namespace mb {
    class Core {
    public:
        enum Type {
            Nes = 0,
            Gb = 1,
            Sms = 2
        };

        explicit Core(Platform *platform) {
            p_platform = platform;
        }

        virtual ~Core() = default;

        virtual bool loop(uint16_t buttons);

        virtual bool loadRom(Io::FileBuffer file) { return false; }

        virtual std::string getSramPath() { return m_sramPath; };

        virtual Platform *getPlatform() { return p_platform; }

    protected:
        Platform *p_platform;
        std::string m_romName;
        std::string m_sramPath;
    };
}

#endif //MICROBOY_CORE_H
