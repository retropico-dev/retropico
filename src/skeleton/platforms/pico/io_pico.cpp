//
// Created by cpasjuste on 01/06/23.
//

#include "platform.h"
#include "io_pico.h"
#include "f_util.h"
#include "hw_config.h"

using namespace mb;

PicoIo::PicoIo() : Io() {
    p_sd = sd_get_by_num(0);
}

// TODO: target == Ram
Io::FileBuffer PicoIo::read(const std::string &path, const Target &target) {
    uint8_t buffer[FLASH_SECTOR_SIZE];
    Io::FileBuffer fileBuffer;
    uint32_t offset;
    FSIZE_t size;
    FRESULT fr;
    UINT br;
    FIL fp;

    // mount sdcard
    bool res = mount();
    if (!res) {
        return {};
    }

    // open file for reading
    fr = f_open(&fp, path.c_str(), FA_READ);
    if (FR_OK != fr && FR_EXIST != fr) {
        printf("PicoIo::read: f_open(%s) error: %s (%d)\r\n", path.c_str(), FRESULT_str(fr), fr);
        unmount();
        return fileBuffer;
    }

    // get file size
    size = f_size(&fp);

    // set target flash offset
    if (target == Target::Flash) {
        if (m_flash_offset_misc + size > PICO_FLASH_SIZE_BYTES) {
            printf("PicoIo::read: error: flash is full... (size: 0x%08llX, offset: 0x%08llX)\r\n",
                   size, m_flash_offset_misc + size);
            f_close(&fp);
            unmount();
            return {};
        }
        offset = m_flash_offset_misc;
        m_flash_offset_misc += ((size + FLASH_SECTOR_SIZE - 1) / size) * size;
    }

    stdio_flush();
    printf("\r\nPicoIo::read: %s, writing file to flash (size: 0x%08llX, offset: 0x%08lX) ...",
           path.c_str(), size, offset);

    for (uint32_t i = 0; i < size; i += FLASH_SECTOR_SIZE) {
        //printf(".");
        //stdio_flush();
        fr = f_read(&fp, buffer, FLASH_SECTOR_SIZE, &br);
        if (FR_OK != fr) {
            printf("\r\nPicoIo::read: f_read error: %s (%d)\r\n", FRESULT_str(fr), fr);
            break;
        }

        writeSector(offset + i, buffer);
    }

    // sync
    f_sync(&fp);

    // close file
    f_close(&fp);

    // unmount sdcard
    unmount();

    // set return data
    fileBuffer.data = (uint8_t *) (XIP_BASE + offset);
    fileBuffer.size = size;

    printf(" done\r\n");
    stdio_flush();

    return fileBuffer;
}

Io::FileBuffer PicoIo::readRomFromFlash() {
    Io::FileBuffer fb{
            .data = (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET_ROM_DATA),
            .size = 1024 * 1024
    };
    memcpy(fb.name, (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET_ROM_HEADER), IO_MAX_PATH);
    return fb;
}

bool PicoIo::writeRomToFlash(const std::string &path, const std::string &name) {
    uint32_t offset = FLASH_TARGET_OFFSET_ROM_DATA;
    uint8_t buffer[FLASH_SECTOR_SIZE];
    FSIZE_t size;
    FRESULT fr;
    UINT br;
    FIL fp;

    // mount sdcard
    bool res = mount();
    if (!res) {
        return false;
    }

    // open file for reading
    fr = f_open(&fp, path.c_str(), FA_READ);
    if (FR_OK != fr && FR_EXIST != fr) {
        printf("PicoIo::writeRomToFlash: f_open(%s) error: %s (%d)\r\n", path.c_str(), FRESULT_str(fr), fr);
        unmount();
        return false;
    }

    // get file size
    size = f_size(&fp);
    if (size < FLASH_SECTOR_SIZE) {
        printf("PicoIo::writeRomToFlash: size error: %llu\r\n", size);
        f_close(&fp);
        unmount();
        return false;
    }

    stdio_flush();
    printf("\r\nPicoIo::writeRomToFlash: %s, writing file to flash (size: 0x%08llX (%llu), offset: 0x%08lX) ...",
           path.c_str(), size, size, offset);
    stdio_flush();

    // write header (rom name)
    memset(buffer, 0, FLASH_SECTOR_SIZE);
    memcpy(buffer, name.c_str(), IO_MAX_PATH);
    writeSector(FLASH_TARGET_OFFSET_ROM_HEADER, buffer);

    // write rom data
    for (uint32_t i = 0; i < size; i += FLASH_SECTOR_SIZE) {
        //printf(".");
        //stdio_flush();
        fr = f_read(&fp, buffer, FLASH_SECTOR_SIZE, &br);
        if (FR_OK != fr) {
            printf("\r\nPicoIo::writeRomToFlash: f_read error: %s (%d)\r\n", FRESULT_str(fr), fr);
            break;
        }
        writeSector(offset + i, buffer);
    }

    // sync
    f_sync(&fp);

    // close file
    f_close(&fp);

    // unmount sdcard
    unmount();

    printf(" done\r\n");
    stdio_flush();

    return true;
}

bool PicoIo::write(const std::string &path, const Io::FileBuffer &fileBuffer) {
    FRESULT fr;
    UINT br;
    FIL fp;

    // mount sdcard
    bool res = mount();
    if (!res) {
        return {};
    }

    // open file for reading
    fr = f_open(&fp, path.c_str(), FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
        printf("PicoIo::write: f_open(%s) error: %s (%d)\n", path.c_str(), FRESULT_str(fr), fr);
        unmount();
        return false;
    }

    stdio_flush();
    printf("\r\nPicoIo::write: %s, writing file to sdcard (size: %zu) ", path.c_str(), fileBuffer.size);

    fr = f_write(&fp, fileBuffer.data, fileBuffer.size, &br);
    if (FR_OK != fr) {
        printf("\r\nPicoIo::write: f_write error: %s (%d)\n", FRESULT_str(fr), fr);
        res = false;
    }

    // sync
    f_sync(&fp);

    // close file
    f_close(&fp);

    // unmount sdcard
    unmount();

    printf(" done\r\n");
    stdio_flush();

    return res;
}

// load array of filenames to flash for memory reduction
Io::FileListBuffer PicoIo::getDir(const std::string &path) {
    // static getDir filename allocation
    static char m_files_buffer[FLASH_SECTOR_SIZE / IO_MAX_PATH][IO_MAX_PATH];
    Io::FileListBuffer fileListBuffer;
    uint32_t offsetBase = m_flash_offset_misc;
    uint32_t fileCountTotal = 0;
    uint32_t fileCountCurrent = 0;
    FILINFO fno;
    FRESULT fr;
    DIR dir;

    // mount sdcard
    bool res = mount();
    if (!res) {
        return fileListBuffer;
    }

    fr = f_opendir(&dir, path.c_str());
    if (fr == FR_OK) {
        // 32 x 128 chars max
        uint32_t maxFiles = FLASH_SECTOR_SIZE / IO_MAX_PATH;
        memset(m_files_buffer, 0, sizeof(m_files_buffer));

        for (;;) {
            if (fileCountTotal >= IO_MAX_FILES) break;

            fr = f_readdir(&dir, &fno);
            if (fr != FR_OK || fno.fname[0] == 0) break;

            if (!(fno.fattrib & AM_DIR)) {
                strncpy(m_files_buffer[fileCountCurrent], fno.fname, IO_MAX_PATH - 1);
                fileCountTotal++;
                fileCountCurrent++;
                if (fileCountCurrent == maxFiles) { // FLASH_SECTOR_SIZE
                    writeSector(m_flash_offset_misc, (uint8_t *) m_files_buffer);
                    m_flash_offset_misc += FLASH_SECTOR_SIZE;
                    fileCountCurrent = 0;
                    memset(m_files_buffer, 0, sizeof(m_files_buffer));
                }
            }
        }

        if (fileCountCurrent > 0) {
            writeSector(m_flash_offset_misc, (uint8_t *) m_files_buffer);
            m_flash_offset_misc += FLASH_SECTOR_SIZE;
        }

        f_closedir(&dir);
    }

    //printf("PicoIo::getDir: m_flash_offset_misc: %i\r\n", m_flash_offset_misc);

    // unmount sdcard
    unmount();

    fileListBuffer.data = (uint8_t *) (XIP_BASE + offsetBase);
    fileListBuffer.count = (int) fileCountTotal;

    return fileListBuffer;
}

void PicoIo::createDir(const std::string &path) {
    const char *p;
    char *temp;
    FRESULT fr;

    // add "/"
    std::string newPath = path;
    if (newPath[newPath.size() - 1] != '/') {
        newPath = newPath + "/";
    }

    // mount sdcard
    bool res = mount();
    if (!res) {
        return;
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
        fr = f_mkdir(temp);
        if (fr != FR_OK && fr != FR_EXIST) {
            break;
        }
    }

    free(temp);

    // unmount sdcard
    unmount();
}

bool PicoIo::mount() {
    FRESULT fr = f_mount(&p_sd->fatfs, p_sd->pcName, 1);
    if (FR_OK != fr) {
        printf("PicoIo::mount: mount error: %s (%d)\r\n", FRESULT_str(fr), fr);
        return false;
    }

    return true;
}

bool PicoIo::unmount() {
    FRESULT fr = f_unmount(p_sd->pcName);
    if (FR_OK != fr) {
        printf("PicoIo::unmount: unmount error: %s (%d)\r\n", FRESULT_str(fr), fr);
        return false;
    }

    return true;
}

void PicoIo::writeSector(uint32_t flash_offs, const uint8_t *data) {
    // disable interrupts...
    uint32_t ints = save_and_disable_interrupts();
    // flash
    flash_range_erase(flash_offs, FLASH_SECTOR_SIZE);
    flash_range_program(flash_offs, data, FLASH_SECTOR_SIZE);
    // restore interrupts...
    restore_interrupts(ints);
}
