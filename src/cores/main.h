//
// Created by cpasjuste on 02/08/23.
//

#ifndef MICROBOY_MAIN_H
#define MICROBOY_MAIN_H

#include "platform.h"
#include "../bootloader/flashloader.h"

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
#define MAX_OVERCLOCK false
#elif MB_GB
#include "peanut-gb.h"
#define MBCore PeanutGB
#define MAX_OVERCLOCK true
#elif MB_SMS
#include "smsplus.h"
#define MBCore SMSPlus
#define MAX_OVERCLOCK false
#endif

#endif //MICROBOY_MAIN_H
