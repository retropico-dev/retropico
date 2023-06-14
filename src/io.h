//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_H
#define MICROBOY_IO_H

#include <cstdint>
#include <string>
#include <vector>

#define IO_MAX_FILES 1024
#define IO_MAX_PATH 32

namespace mb {
    class Io {
    public:
        struct FileBuffer {
            uint8_t *data = nullptr;
            size_t size = 0;

            ~FileBuffer() {
                if (data) free(data);
            }
        };

        Io() = default;

        virtual ~Io() {
            printf("~Io()\n");
        }

        virtual FileBuffer load(const std::string &path) { return {}; }

        virtual std::vector<std::string> getDir(const std::string &path, int maxFiles = IO_MAX_FILES) { return {}; }

        static std::string getRomPath() {
#if MB_GB
            return "/roms/gameboy";
#else
            return "/roms/nes";
#endif
        }
    };
}

#endif //MICROBOY_IO_H
