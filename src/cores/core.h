//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_CORE_H
#define MICROBOY_CORE_H

#include <string>
#include "platform.h"

namespace mb {
    class Core : public P2DPlatform {
    public:
        enum Type {
            Nes = 0,
            Gb = 1,
            Sms = 2
        };

        explicit Core(const p2d::Display::Settings &displaySettings, const Core::Type &core)
                : P2DPlatform(displaySettings) {}

        bool loop() override;

        virtual void close() {}

        virtual bool loadRom(const p2d::Io::File &file) { return false; }

        virtual std::string getSramPath() { return m_sramPath; };

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
        std::string m_romName;
        std::string m_sramPath;
    };
}

#endif //MICROBOY_CORE_H
