//
// Created by cpasjuste on 02/08/23.
//

#ifndef RETROPICO_MAIN_H
#define RETROPICO_MAIN_H

#include "platform.h"
#include "../bootloader/bootloader.h"

#ifdef PICO_BUILD
#if defined(MB_NES) || defined(MB_SMS)
// use direct drawing
#undef P2DDisplay
#define P2DDisplay PicoDisplayDirectDraw
#endif
#endif

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

#endif //RETROPICO_MAIN_H
