//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_CORE_H
#define MICROBOY_CORE_H

#include <string>
#include "platform.h"
#include "ui.h"

namespace mb {
    class Core {
    public:
        explicit Core(Platform *platform/*, Ui *ui*/) {
            p_platform = platform;
            //p_ui = ui;
        }

        virtual ~Core() = default;

        virtual bool loop() { return false; }

        virtual bool loadRom(const std::string &path) { return false; }

        virtual bool loadRom(Io::FileBuffer file) { return false; }

        virtual Platform *getPlatform() { return p_platform; }

    protected:
        Platform *p_platform;
        //Ui *p_ui;
    };
}

#endif //MICROBOY_CORE_H
