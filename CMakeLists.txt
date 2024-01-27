cmake_minimum_required(VERSION 3.5)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

if (NOT PLATFORM_LINUX)
    include(${CMAKE_SOURCE_DIR}/external/misc/libpico2d/src/platforms/pico/cmake/pico_sdk_import.cmake)
endif ()

# pico options
option(PICO_USE_BOOTLOADER "Build bootloader compatible binaries" OFF)
option(PICO_DISPLAY_ST7789_PIO "Pico ST7789 display driver (pio)" ON)
option(PICO_DISPLAY_ALPHA_SUPPORT "Enable alpha/transparency support" OFF)
option(PICO_DEBUG_UART "Pico uart debug" OFF)
option(PICO_DEBUG_USB "Pico usb debug" OFF)

add_subdirectory(${CMAKE_SOURCE_DIR}/external/misc/libpico2d)

project(microboy)

if (NOT PLATFORM_LINUX)
    # i want all executable located at root of build directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE STRING "" FORCE)
endif ()

# microboy-ui (ui binary)
add_subdirectory(src/ui)

# microboy-nes (infones core)
add_subdirectory(src/cores/InfoNES)

# microboy-gb (Peanut-GB core)
add_subdirectory(src/cores/Peanut-GB)

# microboy-sms (smsplus core)
add_subdirectory(src/cores/smsplus)

# microboy-bootloader
if (PICO_USE_BOOTLOADER)
    add_subdirectory(src/bootloader)
endif ()
