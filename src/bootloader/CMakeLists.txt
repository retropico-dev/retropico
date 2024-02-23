cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(microboy-bootloader)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# set source files
file(GLOB BOOTLOADER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/flashloader.c)
set(BOOTLOADER_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
set(BOOTLOADER_LIBS
        hardware_structs
        hardware_sync
        hardware_flash
        hardware_watchdog
        hardware_resets
        hardware_xosc
        hardware_clocks
        hardware_pll
        hardware_dma
        pico_platform
        pico_standard_link
        pico_divider
)
set(BOOTLOADER_FLAGS -Wall -Wextra -Wno-ignored-qualifiers -Os)

# microboy board revision define
if (PICO_BOARD STREQUAL "adafruit_feather_rp2040")
    list(APPEND BOOTLOADER_FLAGS -DMICROBOY_10)
else ()
    list(APPEND BOOTLOADER_FLAGS -DMICROBOY_11)
endif ()

######################
# bootloader binary
######################
add_executable(${PROJECT_NAME} ${BOOTLOADER_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOTLOADER_INCLUDES})
target_compile_options(${PROJECT_NAME} PRIVATE ${BOOTLOADER_FLAGS})
target_link_libraries(${PROJECT_NAME} ${BOOTLOADER_LIBS})
if (NOT PLATFORM_LINUX)
    set_linker_script(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/memmap_flashloader.ld)
    pico_add_uf2_output(${PROJECT_NAME})
    pico_enable_stdio_usb(${PROJECT_NAME} 0)
    pico_enable_stdio_uart(${PROJECT_NAME} 0)

    # combine the bootloader (flashloader) and apps into one UF2 image
    set(COMPLETE_UF2 ${CMAKE_BINARY_DIR}/microboy.uf2)

    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_custom_command(OUTPUT ${COMPLETE_UF2}
            DEPENDS ${CMAKE_PROJECT_NAME}-bootloader ${CMAKE_PROJECT_NAME}-ui ${CMAKE_PROJECT_NAME}-nes ${CMAKE_PROJECT_NAME}-gb ${CMAKE_PROJECT_NAME}-sms
            COMMENT "Building full UF2 image"
            COMMAND ${Python3_EXECUTABLE}
            ${CMAKE_CURRENT_SOURCE_DIR}/uf2tool.py
            -o ${COMPLETE_UF2}
            ${CMAKE_BINARY_DIR}/src/bootloader/${CMAKE_PROJECT_NAME}-bootloader.uf2
            ${CMAKE_BINARY_DIR}/src/ui/${CMAKE_PROJECT_NAME}-ui.uf2
            ${CMAKE_BINARY_DIR}/src/cores/InfoNES/${CMAKE_PROJECT_NAME}-nes.uf2
            ${CMAKE_BINARY_DIR}/src/cores/Peanut-GB/${CMAKE_PROJECT_NAME}-gb.uf2
            ${CMAKE_BINARY_DIR}/src/cores/smsplus/${CMAKE_PROJECT_NAME}-sms.uf2
    )
    add_custom_target(microboy ALL DEPENDS ${COMPLETE_UF2})
endif ()
