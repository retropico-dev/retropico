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
#ifdef LINUX
            std::vector<std::string> data;
#else
            uint8_t *data = nullptr;
#endif
            int count = 0;

            [[nodiscard]] char *get(int idx) const {
#ifdef LINUX
                return const_cast<char *>(data.at(idx).c_str());
#else
                return (char *) &data[idx * IO_MAX_PATH];
#endif
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

        static std::string getRomPath(int core) {
            if (core == 0) {
                return "/roms/nes";
            } else if (core == 1) {
                return "/roms/gameboy";
            } else {
                return "/roms/sms";
            }
        }

        static std::string getSavePath(int core) {
            if (core == 0) {
                return "/saves/nes";
            } else if (core == 1) {
                return "/saves/gameboy";
            } else {
                return "/saves/sms";
            }
        }
    };
}

#endif //MICROBOY_IO_H
