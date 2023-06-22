//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_H
#define MICROBOY_IO_H

#include <cstdint>
#include <string>
#include <vector>

#define IO_MAX_FILES 2048
#define IO_MAX_PATH 128

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

        struct FileListBuffer {
            uint8_t *data = nullptr;
            int count = 0;

            [[nodiscard]] char *get(int idx) const {
                return (char *) &data[idx * IO_MAX_PATH];
            }
        };

        Io() = default;

        virtual ~Io() {
            printf("~Io()\n");
        }

        virtual FileBuffer read(const std::string &path, const Target &target = FlashMisc) { return {}; }

        virtual bool write(const std::string &path, const FileBuffer &fileBuffer) { return false; }

        virtual FileListBuffer getDir(const std::string &path) { return {}; }

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
