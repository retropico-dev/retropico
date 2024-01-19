cmake_minimum_required(VERSION 3.5)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

if (NOT PLATFORM_LINUX)
    include(${CMAKE_SOURCE_DIR}/external/misc/libpico2d/src/platforms/pico/cmake/pico_sdk_import.cmake)
endif ()

add_subdirectory(${CMAKE_SOURCE_DIR}/external/misc/libpico2d)

# pico options
option(PICO_USE_BOOTLOADER "Build bootloader compatible binaries" OFF)

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
