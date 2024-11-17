cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(retropico-nes)

# set source files
file(GLOB NES_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/InfoNES/infones/*.cpp
)
set(NES_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/external/cores/InfoNES/infones
        ${CMAKE_SOURCE_DIR}/external/cores/InfoNES/pico_lib
)
set(NES_FLAGS -DMB_NES
        -DPICO_AUDIO_I2S_MONO_INPUT
        -DPICO_AUDIO_I2S_MONO_OUTPUT
        -DPICO_AUDIO_I2S_BUFFER_SAMPLE_LENGTH=256
)

####################
# InfoNES binary
####################
add_executable(${PROJECT_NAME} ${NES_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${NES_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${NES_FLAGS})
target_link_libraries(${PROJECT_NAME} ${NES_LIBS} retropico-common)

# test rom
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_add_resources(${PROJECT_NAME} "romfs/rom.bin")
endif ()

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        retropico_setup_ld(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/bootloader/memmap_nes.ld")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${GB_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        target_set_pico2d_stuff(${PROJECT_NAME})
    endif ()
endif ()
