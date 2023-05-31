//
// Created by cpasjuste on 30/05/23.
//

#ifndef MICROBOY_PLATFORM_H
#define MICROBOY_PLATFORM_H

#include "display.h"

namespace mb {
    class Platform {
    public:
        Platform();

        Display *getDisplay() { return p_display; };

    protected:
        Display *p_display = nullptr;
    };
}

#endif //MICROBOY_PLATFORM_H
