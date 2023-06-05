//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_LINUX_H
#define MICROBOY_IO_LINUX_H

#include "io.h"

namespace mb {
    class LinuxIo : public Io {
    public:
        uint8_t *load(const std::string &romPath) override;
    };
}

#endif //MICROBOY_IO_LINUX_H