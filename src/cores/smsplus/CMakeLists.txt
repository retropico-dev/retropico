cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(microboy-sms)

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
add_executable(microboy-sms ${SMS_SOURCES})
target_include_directories(microboy-sms PUBLIC ${SMS_INCLUDES})
target_compile_definitions(microboy-sms PUBLIC ${SMS_FLAGS})
target_link_libraries(microboy-sms ${SMS_LIBS} pico2d-skeleton)

# test rom
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    file(GLOB ROMFS romfs/*)
    target_add_resources(microboy-sms ${ROMFS})
endif ()

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        set_linker_script(microboy-sms memmap_sms.ld)
        target_compile_definitions(microboy-sms PUBLIC ${SMS_FLAGS} PICO_USE_BOOTLOADER)
    endif ()
    pico_add_uf2_output(microboy-sms)
    # debug output
    if (PICO_STDIO_UART)
        pico_enable_stdio_usb(microboy-sms 0)
        pico_enable_stdio_uart(microboy-sms 1)
    else ()
        pico_enable_stdio_usb(microboy-sms 1)
        pico_enable_stdio_uart(microboy-sms 0)
    endif ()
endif ()
