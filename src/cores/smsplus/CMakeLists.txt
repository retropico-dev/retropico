cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(retropico-sms)

# set source files
file(GLOB SMS_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/smsplus/*.c
)
set(SMS_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_CURRENT_SOURCE_DIR}/smsplus
)
set(SMS_FLAGS -DMB_SMS -DLSB_FIRST=0
        -DPICO_AUDIO_I2S_BUFFER_SAMPLE_LENGTH=735
)

####################
# smsplus binary
####################
add_executable(${PROJECT_NAME} ${SMS_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SMS_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${SMS_FLAGS})
target_link_libraries(${PROJECT_NAME} ${SMS_LIBS} retropico-common)

# test rom
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_add_resources(${PROJECT_NAME} "romfs/rom.bin")
endif ()

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        set_linker_script(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/bootloader/memmap_sms.ld)
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${SMS_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        set_linker_flash_size_16(${PROJECT_NAME})
    endif ()
    pico_add_uf2_output(${PROJECT_NAME})
    # debug output
    if (PICO_DEBUG_UART)
        pico_enable_stdio_usb(${PROJECT_NAME} 0)
        pico_enable_stdio_uart(${PROJECT_NAME} 1)
    elseif (PICO_DEBUG_USB)
        pico_enable_stdio_usb(${PROJECT_NAME} 1)
        pico_enable_stdio_uart(${PROJECT_NAME} 0)
    else ()
        pico_enable_stdio_usb(${PROJECT_NAME} 0)
        pico_enable_stdio_uart(${PROJECT_NAME} 0)
    endif ()
endif ()
