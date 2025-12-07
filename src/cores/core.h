//
// Created by cpasjuste on 06/06/23.
//

#ifndef RETROPICO_CORE_H
#define RETROPICO_CORE_H

#include <string>
#include "platform.h"
#include "retropico_config.h"
#include "retropico_overlay.h"

namespace retropico {
    class Core : public P2DPlatform {
    public:
        enum Type {
            Nes = 0,
            Gb = 1,
            Sms = 2,
            Gg = 3,
            Genesis = 4
        };

        explicit Core(const p2d::Display::Settings &displaySettings, const Core::Type &core);

        bool loop() override;

        virtual void close() {
        }

        virtual bool loadRom(const p2d::Io::File &file) { return false; }

        virtual std::string getSramPath() { return m_sramPath; };

        virtual std::string getSavePath() { return m_savePath; };

        virtual Config *getConfig() { return p_config; }

        virtual Overlay *getOverlay() { return p_overlay; }

        static std::string getRomsPath(int core) {
            if (core == Nes) {
                return "sd:/roms/nes";
            } else if (core == Gb) {
                return "sd:/roms/gb";
            } else if (core == Sms) {
                return "sd:/roms/sms";
            } else if (core == Gg) {
                return "sd:/roms/gg";
            } else {
                return "sd:/roms/md";
            }
        }

        static std::string getSavesPath(int core) {
            if (core == Nes) {
                return "sd:/saves/nes";
            } else if (core == Gb) {
                return "sd:/saves/gb";
            } else if (core == Sms) {
                return "sd:/saves/sms";
            } else if (core == Gg) {
                return "sd:/saves/gg";
            } else {
                return "sd:/saves/md";
            }
        }

    protected:
        Config *p_config;
        Overlay *p_overlay;
        std::string m_romName;
        std::string m_sramPath;
        std::string m_savePath;
    };
}

#endif //RETROPICO_CORE_H
