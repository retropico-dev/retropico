cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

# linux platform
option(PLATFORM_LINUX "Linux target platform" OFF)

# microboy default board is adafruit feather rp2040
if (NOT PLATFORM_LINUX AND NOT PICO_BOARD)
    set(PICO_BOARD "adafruit_feather_rp2040")
endif ()

# pico options
option(PICO_STDIO_UART "Pico UART debug" ON)
option(PICO_USE_BOOTLOADER "Build bootloader compatible binaries" ON)
option(PICO_DISPLAY_ST7789_PIO "Pico ST7789 display driver (pio)" ON)

if (NOT PLATFORM_LINUX)
    # initialize the SDK based on PICO_SDK_PATH
    # note: this must happen before project()
    include(${CMAKE_SOURCE_DIR}/src/skeleton/platforms/pico/cmake/pico_sdk_import.cmake)

    # for pico_audio_i2s, hardware_sleep
    include(${CMAKE_SOURCE_DIR}/src/skeleton/platforms/pico/cmake/pico_extras_import.cmake)
endif ()

project(microboy)

if (NOT PLATFORM_LINUX)
    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()
endif ()

if (NOT PLATFORM_LINUX)
    function(set_linker_script TARGET script)
        target_link_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src/bootloader)
        pico_set_linker_script(${TARGET} ${CMAKE_SOURCE_DIR}/src/bootloader/${script})

        # Add dependencies on the 'included' linker scripts so that the target gets
        # rebuilt if they are changed
        pico_add_link_depend(${TARGET} ${CMAKE_SOURCE_DIR}/src/bootloader/memmap_defines.ld)
        pico_add_link_depend(${TARGET} ${CMAKE_SOURCE_DIR}/src/bootloader/memmap_default.ld)
    endfunction()
endif ()

# microboy-skeleton (cross-platform library)
add_subdirectory(src/skeleton)

# microboy-ui (ui binary)
add_subdirectory(src/ui)

# microboy-nes (infones core)
add_subdirectory(src/cores/InfoNES)

# microboy-gb (Peanut-GB core)
add_subdirectory(src/cores/Peanut-GB)

# microboy-bootloader
if (PICO_USE_BOOTLOADER)
    add_subdirectory(src/bootloader)
endif ()
