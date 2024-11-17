cmake_minimum_required(VERSION 3.5)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(retropico-bootloader)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# set source files
file(GLOB BOOTLOADER_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/bootloader.c
        ${CMAKE_CURRENT_SOURCE_DIR}/bootloader_helper.c
)
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
        pico_stdlib
)
set(BOOTLOADER_FLAGS
        -DPICO_DEFAULT_UART_TX_PIN=16
        -DPICO_DEFAULT_UART_RX_PIN=17
        #-DBTN_PIN_VOL_D=-1
        -Wall -Wextra -Wno-ignored-qualifiers -Os
)

if (NOT PLATFORM_LINUX)
    # INPUTS GPIO
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_VOL_UP" "6")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_VOL_DOWN" "7")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_UP" "8")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_LEFT" "9")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_DOWN" "10")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_RIGHT" "11")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_SELECT" "12")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_START" "13")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_A" "15")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_B" "14")
    add_gpio_definition(BOOTLOADER_FLAGS "GPIO_PIN_BTN_SLEEP" "17")

    # picoprobe debug
    if (PICO_DEBUG_UART)
        list(APPEND BOOTLOADER_FLAGS -DPICO_DEBUG_UART)
    elseif (PICO_DEBUG_USB)
        list(APPEND BOOTLOADER_FLAGS -DPICO_DEBUG_USB)
    endif ()
    if (NOT PICO_FLASH_SIZE_BYTES)
        if (PICO_DEFAULT_FLASH_SIZE_BYTES)
            set(PICO_FLASH_SIZE_BYTES ${PICO_DEFAULT_FLASH_SIZE_BYTES})
        else ()
            set(PICO_FLASH_SIZE_BYTES "2 * 1024 * 1024")
            message(WARNING "PICO_FLASH_SIZE_BYTES not set, using defaults (${PICO_FLASH_SIZE_BYTES})")
        endif ()
    endif ()
endif ()

######################
# bootloader binary
######################
add_executable(${PROJECT_NAME} ${BOOTLOADER_SOURCES})
target_include_directories(${PROJECT_NAME} PRIVATE ${BOOTLOADER_INCLUDES})
target_compile_options(${PROJECT_NAME} PRIVATE ${BOOTLOADER_FLAGS})
target_link_libraries(${PROJECT_NAME} ${BOOTLOADER_LIBS} #[[ pico2d-skeleton ]])

if (NOT PLATFORM_LINUX)
    retropico_setup_ld(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/bootloader/memmap_flashloader.ld")

    # combine the bootloader (flashloader) and apps into one UF2 image
    set(COMPLETE_UF2 ${CMAKE_BINARY_DIR}/retropico.uf2)

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
    add_custom_target(retropico ALL DEPENDS ${COMPLETE_UF2})
endif ()
