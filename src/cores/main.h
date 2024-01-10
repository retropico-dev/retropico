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
#define ROMFS_ROM "romfs/mario.nes"
#elif MB_GB
#include "peanut-gb.h"
#define ROMFS_ROM "romfs/mario.gb"
#define MBCore PeanutGB
#elif MB_SMS
#include "smsplus.h"
#define MBCore SMSPlus
#define ROMFS_ROM "romfs/sonic.sms"
#endif

#endif //MICROBOY_MAIN_H
