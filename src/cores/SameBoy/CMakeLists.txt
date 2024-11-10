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
        message(FATAL_ERROR "TODO")
        #set_linker_script(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/bootloader/memmap_sms.ld)
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${SAMEBOY_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        #set_linker_flash_size_16(${PROJECT_NAME})
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
