//
// Created by cpasjuste on 01/06/23.
//

#include <iostream>
#include <fstream>
#include "platform.h"

using namespace mb;

uint8_t *LinuxIo::load(const std::string &romPath, size_t *size) {
    // remove "/"
    std::string path = romPath;
    path.erase(0, 1);

    printf("LinuxIo::load: %s\n", path.c_str());

    std::ifstream is(path, std::ios::binary | std::ios::ate);
    if (!is.good()) {
        printf("LinuxIo::load: could not open file (%s)\r\n", path.c_str());
        is.close();
        return nullptr;
    }

    std::streamsize len = is.tellg();
    is.seekg(0, std::ios::beg);
    auto buffer = static_cast<uint8_t *>(malloc(len));
    if (!is.read((char *) buffer, len)) {
        printf("LinuxIo::load: could not read file (%s)\r\n", path.c_str());
        free(buffer);
        return nullptr;
    }

    is.close();
    if (*size) {
        *size = len;
    }

    return buffer;
}
