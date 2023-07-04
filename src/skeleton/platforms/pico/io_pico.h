//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_PICO_H
#define MICROBOY_IO_PICO_H

#include "sd_card.h"
#include "bootloader/flashloader.h"

namespace mb {
    class PicoIo : public Io {
    public:
        PicoIo();

        FileBuffer read(const std::string &path, const Target &target = FlashMisc) override;

        bool write(const std::string &path, const FileBuffer &fileBuffer) override;

        FileListBuffer getDir(const std::string &path) override;

        void createDir(const std::string &path) override;

    private:
        sd_card_t *p_sd = nullptr;
        size_t m_flash_offset_misc = FLASH_TARGET_OFFSET_MISC;

        bool mount();

        bool unmount();

        static void writeSector(uint32_t flash_offs, const uint8_t *data);
    };
}

#endif //MICROBOY_IO_PICO_H
