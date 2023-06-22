//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_H
#define MICROBOY_IO_H

#include <cstdint>
#include <string>
#include <vector>

#define IO_MAX_FILES 512

namespace mb {
    class Io {
    public:
        enum Target {
            FlashRomHeader,
            FlashRomData,
            FlashMisc,
            Ram
        };

        struct FileBuffer {
            uint8_t *data = nullptr;
            size_t size = 0;
        };

        Io() = default;

        virtual ~Io() {
            printf("~Io()\n");
        }

        virtual FileBuffer read(const std::string &path, const Target &target = FlashMisc) { return {}; }

        virtual bool write(const std::string &path, const FileBuffer &fileBuffer) { return false; }

        virtual std::vector<std::string> getDir(const std::string &path, int maxFiles = IO_MAX_FILES) { return {}; }

        virtual void createDir(const std::string &path) {};

        static std::string getRomPath() {
#if MB_GB
            return "/roms/gameboy";
#else
            return "/roms/nes";
#endif
        }

        static std::string getSavePath() {
#if MB_GB
            return "/saves/gameboy";
#else
            return "/saves/nes";
#endif
        }
    };
}

#endif //MICROBOY_IO_H
