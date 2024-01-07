cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(microboy-skeleton)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
# TODO: why ?
set(CMAKE_FLAGS_RELEASE_FIX "-fno-move-loop-invariants -fno-devirtualize -fno-devirtualize-speculatively")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_FLAGS_RELEASE_FIX}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_FLAGS_RELEASE_FIX}" CACHE STRING "" FORCE)

# set source files
file(GLOB PLATFORM_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/utility/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/compat/*.c*
        ${CMAKE_SOURCE_DIR}/external/misc/Adafruit-GFX-Library/Adafruit_GFX.cpp
)
set(PLATFORM_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/utility
        ${CMAKE_CURRENT_SOURCE_DIR}/compat
        ${CMAKE_SOURCE_DIR}/src # for bootloader header
        ${CMAKE_SOURCE_DIR}/external/misc/Adafruit-GFX-Library
)

set(PLATFORM_FLAGS "-DNO_PICO_LED -DARDUINO=123 -ffast-math")
if (PICO_STDIO_UART)
    list(APPEND PLATFORM_FLAGS -DPICO_STDIO_UART)
endif ()

if (PLATFORM_LINUX)
    file(GLOB_RECURSE TEMP ${CMAKE_CURRENT_SOURCE_DIR}/platforms/linux/*.c*)
    list(APPEND PLATFORM_SOURCES ${TEMP})
    list(APPEND PLATFORM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/platforms/linux)
    # SDL2
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2)
    list(APPEND PLATFORM_INCLUDES ${SDL2_INCLUDE_DIRS})
    list(APPEND PLATFORM_FLAGS -DLINUX)
    list(APPEND PLATFORM_LIBS ${SDL2_LIBRARIES})
else ()
    file(GLOB TEMP ${CMAKE_CURRENT_SOURCE_DIR}/platforms/pico/*.c*)
    list(APPEND PLATFORM_SOURCES ${TEMP})
    list(APPEND PLATFORM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/platforms/pico)
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
            hardware_flash
    )
    # spi sdcard driver
    add_subdirectory(${CMAKE_SOURCE_DIR}/external/misc/no-OS-FatFS-SD-SPI-RPi-Pico/FatFs_SPI build)
    list(APPEND PLATFORM_LIBS FatFs_SPI)
    # display driver
    if (PICO_DISPLAY_ST7789_PIO)
        list(APPEND PLATFORM_SOURCES
                ${CMAKE_CURRENT_SOURCE_DIR}/platforms/pico/display/st7789/st7789.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/platforms/pico/display/st7789/pico_display.cpp
                ${CMAKE_CURRENT_SOURCE_DIR}/platforms/pico/display/st7789/pico_display_buffered.cpp
        )
        list(APPEND PLATFORM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/platforms/pico/display/st7789)
        list(APPEND PLATFORM_LIBS hardware_dma)
    endif ()
    # microboy board revision define
    if (PICO_BOARD STREQUAL "adafruit_feather_rp2040")
        list(APPEND PLATFORM_FLAGS -DMICROBOY_10)
    else ()
        list(APPEND PLATFORM_FLAGS -DMICROBOY_11 "-DPICO_FLASH_SIZE_BYTES=(16 * 1024 * 1024)")
    endif ()
endif ()

####################
# skeleton library
####################
add_library(microboy-skeleton ${PLATFORM_SOURCES})
target_include_directories(microboy-skeleton PUBLIC ${PLATFORM_INCLUDES})
target_compile_definitions(microboy-skeleton PUBLIC ${PLATFORM_FLAGS})
target_link_libraries(microboy-skeleton ${PLATFORM_LIBS})
if (NOT PLATFORM_LINUX)
    if (PICO_DISPLAY_ST7789_PIO)
        pico_generate_pio_header(microboy-skeleton
                ${CMAKE_CURRENT_SOURCE_DIR}/platforms/pico/display/st7789/st7789_lcd.pio)
    endif ()
    # debug output
    if (PICO_STDIO_UART)
        pico_enable_stdio_usb(microboy-skeleton 0)
        pico_enable_stdio_uart(microboy-skeleton 1)
    else ()
        pico_enable_stdio_usb(microboy-skeleton 1)
        pico_enable_stdio_uart(microboy-skeleton 0)
    endif ()
endif ()
