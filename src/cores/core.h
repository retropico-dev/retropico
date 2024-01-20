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

        virtual bool loadRom(const p2d::File &file) { return false; }

        virtual std::string getSramPath() { return m_sramPath; };

        virtual p2d::Platform *getPlatform() { return p_platform; }

        static std::string getRomCachePath() { return "flash:/rom.bin"; }

        static std::string getRomPath(int core) {
            if (core == Nes) {
                return "sd:/roms/nes";
            } else if (core == Gb) {
                return "sd:/roms/gb";
            } else {
                return "sd:/roms/sms";
            }
        }

        static std::string getSavePath(int core) {
            if (core == Nes) {
                return "sd:/saves/nes";
            } else if (core == Gb) {
                return "sd:/saves/gb";
            } else {
                return "sd:/saves/sms";
            }
        }

    protected:
        p2d::Platform *p_platform;
        std::string m_romName;
        std::string m_sramPath;
    };
}

#endif //MICROBOY_CORE_H
