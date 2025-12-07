cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(retropico-genesis)

# set source files
file(GLOB GEN_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../*.c*
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/bus/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/cpus/M68K/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/cpus/Z80/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/io/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/savestate/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/sound/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/vdp/*.c*
)
set(GEN_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/bus
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/cpus/M68K
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/cpus/Z80
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/io
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/savestate
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/sound
        ${CMAKE_SOURCE_DIR}/external/cores/gwenesis/src/vdp
)
set(GEN_FLAGS
        -DMB_GENESIS
        -DPICO_AUDIO_I2S_BUFFER_SAMPLE_LENGTH=256
)

####################
# Peanut-GB binary
####################
add_executable(${PROJECT_NAME} ${GEN_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${GEN_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${GEN_FLAGS})
target_link_libraries(${PROJECT_NAME} ${GEN_LIBS} retropico-common)

# test rom
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_add_resources(${PROJECT_NAME} "romfs/rom.bin")
endif ()

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        retropico_setup_ld(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/bootloader/memmap_gb.ld")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${GEN_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        target_set_pico2d_stuff(${PROJECT_NAME})
    endif ()
endif ()
