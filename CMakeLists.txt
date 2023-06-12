cmake_minimum_required(VERSION 3.13)

# linux platform
option(PLATFORM_LINUX "Linux target platform" OFF)

# pico options
option(PICO_DISPLAY_ST7789_PIO "Pico ST7789 display driver (pio)" ON)
option(PICO_DISPLAY_ST7789_SPI "Pico ST7789 display driver (spi)" OFF)
option(PICO_DISPLAY_SSD1306 "Pico SSD1306 display driver (debug)" OFF)

if (NOT PLATFORM_LINUX)
    # initialize the SDK based on PICO_SDK_PATH
    # note: this must happen before project()
    include(src/platforms/pico/cmake/pico_sdk_import.cmake)

    # for pico_audio_i2s, hardware_sleep
    include(src/platforms/pico/cmake/pico_extras_import.cmake)
endif ()

project(microboy)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (NOT PLATFORM_LINUX)
    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()
endif ()

# set source files
file(GLOB PLATFORM_SOURCES
        src/*.c*
        src/utility/*.c*
        src/compat/*.c*
        external/misc/Adafruit-GFX-Library/Adafruit_GFX.cpp
        )
set(PLATFORM_INCLUDES
        src
        src/utility
        src/compat
        external/misc/Adafruit-GFX-Library
        )

set(PLATFORM_FLAGS
        -DARDUINO=123
        )

# OK (WIP)
include(src/cores/Peanut-GB/peanut-gb.cmake)
# OK (WIP)
include(src/cores/InfoNES/infones.cmake)
# too slow...
#include(src/cores/sameboy/sameboy.cmake)
# too much memory usage
#include(src/cores/Gearsystem/gearsystem.cmake)

if (PLATFORM_LINUX)
    file(GLOB_RECURSE TEMP src/platforms/linux/*.c*)
    list(APPEND PLATFORM_SOURCES ${TEMP})
    list(APPEND PLATFORM_INCLUDES src/platforms/linux)
    # SDL2
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2)
    list(APPEND PLATFORM_INCLUDES ${SDL2_INCLUDE_DIRS})
    list(APPEND PLATFORM_FLAGS -DLINUX)
    list(APPEND PLATFORM_LIBS ${SDL2_LIBRARIES})
else ()
    file(GLOB TEMP src/platforms/pico/*.c*)
    list(APPEND PLATFORM_SOURCES ${TEMP})
    list(APPEND PLATFORM_INCLUDES src/platforms/pico)
    list(APPEND PLATFORM_FLAGS
            PARAM_ASSERTIONS_DISABLE_ALL=1
            PICO_ENTER_USB_BOOT_ON_EXIT=1
            PICO_STDIO_ENABLE_CRLF_SUPPORT=0
            PICO_STDIO_DEFAULT_CRLF=0
            PICO_PRINTF_SUPPORT_FLOAT=0
            PICO_PRINTF_SUPPORT_EXPONENTIAL=0
            PICO_PRINTF_SUPPORT_LONG_LONG=1
            PICO_PRINTF_SUPPORT_PTRDIFF_T=0
            PICO_AUDIO_I2S_PIO=1
            PICO_AUDIO_I2S_DMA_IRQ=1
            #PICO_DEFAULT_UART_TX_PIN=12
            #PICO_DEFAULT_UART_RX_PIN=13
            )
    list(APPEND PLATFORM_LIBS
            -Wl,--print-memory-usage
            pico_stdlib
            pico_multicore
            pico_audio_i2s
            hardware_sleep
            hardware_pio
            hardware_dma
            #hardware_flash
            )
    # display driver
    if (PICO_DISPLAY_SSD1306)
        list(APPEND PLATFORM_SOURCES
                src/platforms/pico/display/ssd1306/pico_display.cpp
                src/platforms/pico/display/ssd1306/ssd1306.c
                )
        list(APPEND PLATFORM_INCLUDES src/platforms/pico/display/ssd1306)
        list(APPEND PLATFORM_LIBS hardware_i2c)
    elseif (PICO_DISPLAY_ST7789_PIO)
        list(APPEND PLATFORM_SOURCES
                src/platforms/pico/display/st7789_pio/pico_display.cpp
                src/platforms/pico/display/st7789_pio/st7789.cpp
                )
        list(APPEND PLATFORM_INCLUDES src/platforms/pico/display/st7789_pio)
        list(APPEND PLATFORM_LIBS hardware_dma)
    elseif (PICO_DISPLAY_ST7789_SPI)
        list(APPEND PLATFORM_SOURCES
                src/platforms/pico/display/st7789_spi/pico_display.cpp
                src/platforms/pico/display/st7789_spi/st7789.c)
        list(APPEND PLATFORM_INCLUDES src/platforms/pico/display/st7789_spi)
        list(APPEND PLATFORM_LIBS hardware_spi)
    endif ()
endif ()

############
# gameboy
############
add_executable(${CMAKE_PROJECT_NAME}-gameboy ${PLATFORM_SOURCES}
        ${CMAKE_SOURCE_DIR}/src/cores/Peanut-GB/peanut-gb.cpp ${CMAKE_SOURCE_DIR}/src/cores/Peanut-GB/rom.c)
# gameboy (includes)
target_include_directories(${CMAKE_PROJECT_NAME}-gameboy PRIVATE
        ${PLATFORM_INCLUDES} ${CMAKE_SOURCE_DIR}/src/cores/Peanut-GB)
# gameboy (definitions)
target_compile_definitions(${CMAKE_PROJECT_NAME}-gameboy PRIVATE ${PLATFORM_FLAGS} -DMB_GB)
# gameboy (libs)
target_link_libraries(${CMAKE_PROJECT_NAME}-gameboy ${PLATFORM_LIBS} peanut-gb)
# rpi pico build stuff
if (NOT PLATFORM_LINUX)
    if (PICO_DISPLAY_ST7789_PIO)
        pico_generate_pio_header(${CMAKE_PROJECT_NAME}-gameboy
                ${CMAKE_SOURCE_DIR}/src/platforms/pico/display/st7789_pio/st7789_lcd.pio)
    endif ()
    # run from ram
    pico_set_binary_type(${CMAKE_PROJECT_NAME}-gameboy copy_to_ram)
    # enable usb output, disable uart output
    pico_enable_stdio_usb(${CMAKE_PROJECT_NAME}-gameboy 1)
    pico_enable_stdio_uart(${CMAKE_PROJECT_NAME}-gameboy 0)
    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(${CMAKE_PROJECT_NAME}-gameboy)
endif ()

############
# nes
############
add_executable(${CMAKE_PROJECT_NAME}-nes ${PLATFORM_SOURCES}
        src/cores/InfoNES/infones.cpp src/cores/InfoNES/work_meter.cpp src/cores/InfoNES/rom.c)
# nes (includes)
target_include_directories(${CMAKE_PROJECT_NAME}-nes PRIVATE
        ${PLATFORM_INCLUDES} ${CMAKE_SOURCE_DIR}/src/cores/InfoNES)
# nes (definitions)
target_compile_definitions(${CMAKE_PROJECT_NAME}-nes PRIVATE ${PLATFORM_FLAGS} -DMB_NES)
# nes (libs)
target_link_libraries(${CMAKE_PROJECT_NAME}-nes ${PLATFORM_LIBS} infones)
# rpi pico build stuff
if (NOT PLATFORM_LINUX)
    if (PICO_DISPLAY_ST7789_PIO)
        pico_generate_pio_header(${CMAKE_PROJECT_NAME}-nes
                ${CMAKE_SOURCE_DIR}/src/platforms/pico/display/st7789_pio/st7789_lcd.pio)
    endif ()
    # run from ram (no go with InfoNES)
    #pico_set_binary_type(${CMAKE_PROJECT_NAME}-nes copy_to_ram)
    # enable usb output, disable uart output
    pico_enable_stdio_usb(${CMAKE_PROJECT_NAME}-nes 1)
    pico_enable_stdio_uart(${CMAKE_PROJECT_NAME}-nes 0)
    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(${CMAKE_PROJECT_NAME}-nes)
endif ()
