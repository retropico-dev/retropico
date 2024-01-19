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

        explicit Core(p2d::Platform *platform, const Core::Type &core) {
            p_platform = platform;
        }

        virtual ~Core() = default;

        virtual bool loop(uint16_t buttons);

        virtual bool loadRom(p2d::Io::FileBuffer file) { return false; }

        virtual std::string getSramPath() { return m_sramPath; };

        virtual p2d::Platform *getPlatform() { return p_platform; }

        static std::string getRomPath(int core) {
            if (core == 0) {
                return "/roms/nes";
            } else if (core == 1) {
                return "/roms/gameboy";
            } else {
                return "/roms/sms";
            }
        }

        static std::string getSavePath(int core) {
            if (core == 0) {
                return "/saves/nes";
            } else if (core == 1) {
                return "/saves/gameboy";
            } else {
                return "/saves/sms";
            }
        }

    protected:
        p2d::Platform *p_platform;
        std::string m_romName;
        std::string m_sramPath;
    };
}

#endif //MICROBOY_CORE_H
