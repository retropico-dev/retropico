//
// Created by cpasjuste on 01/06/23.
//

#include "platform.h"
#include "io_pico.h"
#include "f_util.h"
#include "hw_config.h"

using namespace mb;

#define FLASH_TARGET_OFFSET (1024 * 1024)

PicoIo::PicoIo() : Io() {
    p_sd = sd_get_by_num(0);
}

Io::FileBuffer PicoIo::load(const std::string &path, const Target &target) {
    uint8_t buffer[FLASH_SECTOR_SIZE];
    Io::FileBuffer fileBuffer;
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
        printf("PicoIo::load: f_open(%s) error: %s (%d)\n", path.c_str(), FRESULT_str(fr), fr);
        unmount();
        return fileBuffer;
    }

    // get file size
    size = f_size(&fp);

    stdio_flush();
    printf("\r\nPicoIo::load: %s, writing file to flash (size: %llu) ", path.c_str(), size);

    for (uint32_t i = 0; i < size; i += FLASH_SECTOR_SIZE) {
        printf(".");
        stdio_flush();

        fr = f_read(&fp, buffer, FLASH_SECTOR_SIZE, &br);
        if (FR_OK != fr) {
            printf("\r\nPicoIo::load: f_read error: %s (%d)\n", FRESULT_str(fr), fr);
            break;
        }

        // disable interrupts...
        uint32_t ints = save_and_disable_interrupts();
        // flash
        flash_range_erase(FLASH_TARGET_OFFSET + i, FLASH_SECTOR_SIZE);
        flash_range_program(FLASH_TARGET_OFFSET + i, buffer, FLASH_SECTOR_SIZE);
        // restore interrupts...
        restore_interrupts(ints);
    }

    // sync
    f_sync(&fp);

    // close file
    f_close(&fp);

    // unmount sdcard
    unmount();

    // set return data
    fileBuffer.data = (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
    fileBuffer.size = size;

    printf(" done\r\n");
    stdio_flush();

    return fileBuffer;
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
