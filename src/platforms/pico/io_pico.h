//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_PICO_H
#define MICROBOY_IO_PICO_H

namespace mb {
    class PicoIo : public Io {
    public:
        uint8_t *load(const std::string &romPath, size_t *size) override;
    };
}

#endif //MICROBOY_IO_PICO_H
