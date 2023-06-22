//
// Created by cpasjuste on 01/06/23.
//

#ifndef MICROBOY_IO_LINUX_H
#define MICROBOY_IO_LINUX_H

namespace mb {
    class LinuxIo : public Io {
    public:
        FileBuffer read(const std::string &path, const Target &target = FlashMisc) override;

        bool write(const std::string &path, const FileBuffer &fileBuffer) override;

        std::vector<std::string> getDir(const std::string &path, int maxFiles = IO_MAX_FILES) override;

        void createDir(const std::string &path) override;
    };
}

#endif //MICROBOY_IO_LINUX_H
