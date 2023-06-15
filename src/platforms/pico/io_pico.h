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

        FileBuffer load(const std::string &path, const Target &target = Flash) override;

    private:
        sd_card_t *p_sd = nullptr;

        bool mount();

        bool unmount();
    };
}

#endif //MICROBOY_IO_PICO_H
