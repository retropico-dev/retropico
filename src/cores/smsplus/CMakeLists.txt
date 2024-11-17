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
        retropico_setup_ld(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/bootloader/memmap_sms.ld")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${SMS_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        target_set_pico2d_stuff(${PROJECT_NAME})
    endif ()
endif ()
