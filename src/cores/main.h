//
// Created by cpasjuste on 02/08/23.
//

#ifndef MICROBOY_MAIN_H
#define MICROBOY_MAIN_H

#ifdef MB_NES
#include "infones.h"
#define MBCore InfoNES
#elif MB_GB
#include "peanut-gb.h"
#define MBCore PeanutGB
#elif MB_SMS
#include "smsplus.h"
#define MBCore SMSPlus
#endif

#endif //MICROBOY_MAIN_H
