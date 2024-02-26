//
// Created by cpasjuste on 06/06/23.
//

#ifndef RETROPICO_CORE_H
#define RETROPICO_CORE_H

#include <string>
#include "platform.h"

namespace retropico {
    class Core : public P2DPlatform {
    public:
        enum Type {
            Nes = 0,
            Gb = 1,
            Sms = 2,
            Gg = 3
        };

        explicit Core(const p2d::Display::Settings &displaySettings, const Core::Type &core)
                : P2DPlatform(displaySettings) {}

        bool loop() override;

        virtual void close() {}

        virtual bool loadRom(const p2d::Io::File &file) { return false; }

        virtual std::string getSramPath() { return m_sramPath; };

        virtual std::string getSavePath() { return m_savePath; };

        static std::string getRomsPath(int core) {
            if (core == Nes) {
                return "sd:/roms/nes";
            } else if (core == Gb) {
                return "sd:/roms/gb";
            } else if (core == Sms) {
                return "sd:/roms/sms";
            } else {
                return "sd:/roms/gg";
            }
        }

        static std::string getSavesPath(int core) {
            if (core == Nes) {
                return "sd:/saves/nes";
            } else if (core == Gb) {
                return "sd:/saves/gb";
            } else if (core == Sms) {
                return "sd:/saves/sms";
            } else {
                return "sd:/saves/gg";
            }
        }

    protected:
        std::string m_romName;
        std::string m_sramPath;
        std::string m_savePath;
    };
}

#endif //RETROPICO_CORE_H
