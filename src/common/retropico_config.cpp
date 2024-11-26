//
// Created by cpasjuste on 26/02/24.
//

#include "retropico_config.h"

#define RETROPICO_CFG_PATH "flash:/config.bin"

using namespace p2d;
using namespace retropico;

Config::Config() {
    Io::init();

    if (Io::fileExists(RETROPICO_CFG_PATH)) {
        Io::File file{RETROPICO_CFG_PATH};
        if (file.isOpen()) {
            printf("Config::Config: reading %s\r\n", RETROPICO_CFG_PATH);
            file.read(0, sizeof(Data), reinterpret_cast<char *>(&m_data));
            printf("Config::Config: volume: %i\r\n", m_data.volume);
            printf("Config::Config: brightness: %i\r\n", m_data.brightness);
            printf("Config::Config: current filer core: %i\r\n", m_data.filer_current_core);
            printf("Config::Config: current filer core index: %i\r\n", m_data.filer_current_core_index);
        }
    } else {
        printf("Config::Config: config file not found, using defaults... (%s)\r\n", RETROPICO_CFG_PATH);
    }
}

bool Config::save() {
    Io::File file{RETROPICO_CFG_PATH, Io::File::OpenMode::Write};
    if (!file.isOpen()) {
        printf("Config::save: ERROR - could not open file for writing (%s)\r\n", RETROPICO_CFG_PATH);
        return false;
    }

    int32_t len = file.write(0, sizeof(Data), reinterpret_cast<const char *>(&m_data));
    if (len != sizeof(Data)) {
        printf("Config::save: ERROR - could not write to config file (%s)\r\n", RETROPICO_CFG_PATH);
        return false;
    }

    printf("Config::save: config saved successfully to %s\r\n", RETROPICO_CFG_PATH);

    return true;
}
