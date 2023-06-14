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
        explicit Core(Platform *platform) {
            p_platform = platform;
        }

        virtual ~Core() = default;

        virtual bool loop() { return false; }

        virtual bool loadRom(const std::string &path) { return false; }

        virtual bool loadRom(Io::FileBuffer file) { return false; }

        virtual Platform *getPlatform() { return p_platform; }

    protected:
        Platform *p_platform = nullptr;
    };
}

#endif //MICROBOY_CORE_H
