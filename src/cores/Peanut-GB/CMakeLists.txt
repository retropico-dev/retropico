cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(retropico-gb)

# set source files
file(GLOB GB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
)
set(GB_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/external/cores/Peanut-GB
)
set(GB_FLAGS
        -DMB_GB
        -DPICO_AUDIO_I2S_BUFFER_SAMPLE_LENGTH=256
)

####################
# Peanut-GB binary
####################
add_executable(${PROJECT_NAME} ${GB_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${GB_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${GB_FLAGS})
target_link_libraries(${PROJECT_NAME} ${GB_LIBS} retropico-common)

# test rom
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_add_resources(${PROJECT_NAME} "romfs/rom.bin")
endif ()

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        retropico_setup_ld(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/bootloader/memmap_gb.ld")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${GB_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        target_set_pico2d_stuff(${PROJECT_NAME})
    endif ()
endif ()
