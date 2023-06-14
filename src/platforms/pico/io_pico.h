//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_PICO_H
#define MICROBOY_IO_PICO_H

#include "sd_card.h"

namespace mb {
    class PicoIo : public Io {
    public:
        PicoIo();

        uint8_t *load(const std::string &romPath, size_t *size) override;

    private:
        sd_card_t *p_sd = nullptr;

        bool mount();

        bool unmount();
    };
}

#endif //MICROBOY_IO_PICO_H
