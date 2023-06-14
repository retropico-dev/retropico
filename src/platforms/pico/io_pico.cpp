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

uint8_t *PicoIo::load(const std::string &romPath, size_t *size) {
    uint8_t buffer[FLASH_SECTOR_SIZE];
    FSIZE_t fs;
    FRESULT fr;
    UINT br;
    FIL fp;

    // mount sdcard
    bool res = mount();
    if (!res) {
        return nullptr;
    }

    // open file for reading
    fr = f_open(&fp, romPath.c_str(), FA_READ);
    if (FR_OK != fr && FR_EXIST != fr) {
        printf("PicoIo::load: f_open(%s) error: %s (%d)\n", romPath.c_str(), FRESULT_str(fr), fr);
        unmount();
        return nullptr;
    }

    // get file size
    fs = f_size(&fp);

    printf("\r\nPicoIo::load: %s, writing file to flash (size: %llu) ", romPath.c_str(), fs);
    for (uint32_t i = 0; i < fs; i += FLASH_SECTOR_SIZE) {
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

    // set file size if needed
    if (*size) *size = fs;

    stdio_flush();
    printf(" done\r\n");

    return (uint8_t *) (XIP_BASE + FLASH_TARGET_OFFSET);
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
