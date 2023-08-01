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

Io::FileBuffer LinuxIo::read(const std::string &path, const Target &target) {
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

bool LinuxIo::write(const std::string &path, const Io::FileBuffer &fileBuffer) {
    // remove "/"
    std::string newPath = path;
    if (newPath[0] == '/') newPath.erase(0, 1);

    std::ofstream os(newPath, std::ios::binary | std::ios::out);
    if (!os.good()) {
        printf("LinuxIo::write: could not open file (%s)\r\n", newPath.c_str());
        os.close();
        return false;
    }

    if (!os.write((const char *) fileBuffer.data, (std::streamsize) fileBuffer.size)) {
        printf("LinuxIo::write: could not write to file (%s)\r\n", newPath.c_str());
        return false;
    }

    os.close();

    return true;
}

Io::FileListBuffer LinuxIo::getDir(const std::string &path) {
    struct stat st{};
    struct dirent *dir;
    int fileCount = 0;
    Io::FileListBuffer fileListBuffer;

    // remove "/"
    std::string newPath = path;
    if (newPath[0] == '/') newPath.erase(0, 1);

    DIR *d = opendir(newPath.c_str());
    if (d) {
        while ((dir = readdir(d)) != nullptr) {
            if (fileCount >= IO_MAX_FILES) break;

            std::string filePath = newPath + "/" + dir->d_name;
            if (stat(filePath.c_str(), &st) == 0 && S_ISREG(st.st_mode)) {
                fileListBuffer.data.emplace_back(dir->d_name);
                fileCount++;
            }
        }
        closedir(d);
    }

    fileListBuffer.count = fileCount;

    return fileListBuffer;
}

void LinuxIo::createDir(const std::string &path) {
    const char *p;
    char *temp;
    // remove "/"
    std::string newPath = path;
    if (newPath[0] == '/') newPath.erase(0, 1);
    if (newPath[newPath.size() - 1] != '/') {
        newPath = newPath + "/";
    }

    temp = static_cast<char *>(calloc(1, strlen(newPath.c_str()) + 1));
    p = newPath.c_str();

    while ((p = strchr(p, '/')) != nullptr) {
        if (p != newPath.c_str() && *(p - 1) == '/') {
            p++;
            continue;
        }
        memcpy(temp, newPath.c_str(), p - newPath.c_str());
        temp[p - newPath.c_str()] = '\0';
        p++;
        if (mkdir(temp, 0774) != 0) {
            if (errno != EEXIST) {
                break;
            }
        }
    }

    free(temp);
}

bool LinuxIo::writeRomToFlash(const std::string &path, const std::string &name) {
    FILE *fp = fopen("/tmp/microboy.flash", "w");
    if (!fp) return false;
    fwrite(path.c_str(), path.length(), 1, fp);
    fclose(fp);
    return true;
}

Io::FileBuffer LinuxIo::readRomFromFlash() {
    FileBuffer fb;
    FILE *fp = fopen("/tmp/microboy.flash", "r");
    if (fp) {
        std::string str(256, '\0');
        size_t len = fread(&str[0], sizeof(char), (size_t) 256, fp);
        str.resize(len + 1);
        str[len + 1] = '\0';
        // remove "/"
        if (str[0] == '/') str.erase(0, 1);
        // open rom file
        fb = read(str);
        strncpy(fb.name, Utility::baseName(str).c_str(), 128);
    }

    return fb;
}
