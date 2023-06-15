//
// Created by cpasjuste on 01/06/23.
//

#include <iostream>
#include <fstream>
#include <dirent.h>
#include <sys/stat.h>
#include "platform.h"
#include "io_linux.h"

using namespace mb;

Io::FileBuffer LinuxIo::load(const std::string &path, const Target &target) {
    Io::FileBuffer fileBuffer;

    // remove "/"
    std::string newPath = path;
    if (newPath[0] == '/') newPath.erase(0, 1);

    printf("LinuxIo::load: %s\n", newPath.c_str());

    std::ifstream is(newPath, std::ios::binary | std::ios::ate);
    if (!is.good()) {
        printf("LinuxIo::load: could not open file (%s)\r\n", newPath.c_str());
        is.close();
        return fileBuffer;
    }

    std::streamsize size = is.tellg();
    is.seekg(0, std::ios::beg);
    auto data = static_cast<uint8_t *>(malloc(size));
    if (!is.read((char *) data, size)) {
        printf("LinuxIo::load: could not read file (%s)\r\n", newPath.c_str());
        free(data);
        return fileBuffer;
    }

    is.close();

    fileBuffer.data = data;
    fileBuffer.size = size;

    return fileBuffer;
}

std::vector<std::string> LinuxIo::getDir(const std::string &path, int maxFiles) {
    struct stat st{};
    struct dirent *dir;
    std::vector<std::string> files;
    // remove "/"
    std::string newPath = path;
    if (newPath[0] == '/') newPath.erase(0, 1);

    DIR *d = opendir(newPath.c_str());
    if (d) {
        while ((dir = readdir(d)) != nullptr) {
            if (files.size() >= maxFiles) break;

            std::string filePath = newPath + "/" + dir->d_name;
            if (stat(filePath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                files.emplace_back(dir->d_name);
            }
        }
        closedir(d);
    }

    return files;
}
