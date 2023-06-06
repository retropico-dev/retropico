//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_H
#define MICROBOY_IO_H

#include <cstdint>
#include <string>

namespace mb {
    class Io {
    public:
        Io() = default;

        virtual ~Io() {
            printf("~Io()\n");
        }

        virtual uint8_t *load(const std::string &romPath) { return nullptr; };
    };
}

#endif //MICROBOY_IO_H
