//
// Created by cpasjuste on 02/08/23.
//

#ifndef MICROBOY_MAIN_H
#define MICROBOY_MAIN_H

#include "platform.h"
#include "../bootloader/flashloader.h"

#ifdef MB_NES
#include "infones.h"
#define MBCore InfoNES
#define MAX_OVERCLOCK false
#define ROMFS_ROM "romfs/rom.nes"
#elif MB_GB
#include "peanut-gb.h"
#define MBCore PeanutGB
#define MAX_OVERCLOCK true
#define ROMFS_ROM "romfs/rom.gb"
#elif MB_SMS
#include "smsplus.h"
#define MBCore SMSPlus
#define MAX_OVERCLOCK false
#define ROMFS_ROM "romfs/rom.sms"
#endif

#endif //MICROBOY_MAIN_H
