//
// Created by cpasjuste on 06/06/23.
//

#ifndef MICROBOY_ARDUINO_H
#define MICROBOY_ARDUINO_H

/* Arduino to STL C++ */
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <string>

/* disable some Arduino specific memory access MACROS */
#define PROGMEM
#define F(X) X

/* Define some Arduino specific types */
typedef bool boolean;
typedef std::string String;
typedef std::string __FlashStringHelper;

#endif //MICROBOY_ARDUINO_H
