cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(retropico-sameboy)

# set source files
file(GLOB CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/*.c
)
list(REMOVE_ITEM CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/rewind.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/debugger.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/sm83_disassembler.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/symbol_hash.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/cheats.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/cheat_search.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/printer.c
)

file(GLOB SAMEBOY_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/../*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
)
set(SAMEBOY_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy
)
set(SAMEBOY_FLAGS
        -DMB_SAMEBOY
        -DGB_VERSION="10"
        -DGB_COPYRIGHT_YEAR="2023"
        -DGB_DISABLE_TIMEKEEPING
        -DGB_DISABLE_REWIND
        -DGB_DISABLE_DEBUGGER
        -DGB_DISABLE_CHEATS
        -DPICO_AUDIO_I2S_BUFFER_SAMPLE_LENGTH=735
        -DGB_INTERNAL
)

####################
# sameboy binary
####################
add_executable(${PROJECT_NAME} ${CORE_SRC} ${SAMEBOY_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${SAMEBOY_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${SAMEBOY_FLAGS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wno-multichar)
target_link_libraries(${PROJECT_NAME} ${SAMEBOY_LIBS} retropico-common)

# test rom
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_add_resources(${PROJECT_NAME} "romfs/rom.bin")
endif ()

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        retropico_setup_ld(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/bootloader/memmap_gb.ld")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${SAMEBOY_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        target_set_pico2d_stuff(${PROJECT_NAME})
    endif ()
endif ()
