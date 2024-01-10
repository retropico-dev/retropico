cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(microboy-nes)

# TODO: why ?
set(CMAKE_FLAGS_RELEASE_FIX "-fno-move-loop-invariants -fno-devirtualize -fno-devirtualize-speculatively")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_FLAGS_RELEASE_FIX}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_FLAGS_RELEASE_FIX}" CACHE STRING "" FORCE)

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
        -DPICO_AUDIO_I2S_BUFFER_SAMPLE_LENGTH=735
)

####################
# InfoNES binary
####################
add_executable(microboy-nes ${NES_SOURCES})
target_include_directories(microboy-nes PUBLIC ${NES_INCLUDES})
target_compile_definitions(microboy-nes PUBLIC ${NES_FLAGS})
target_link_libraries(microboy-nes ${NES_LIBS} pico2d-skeleton)
if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        set_linker_script(microboy-nes memmap_nes.ld)
        target_compile_definitions(microboy-nes PUBLIC ${NES_FLAGS} PICO_USE_BOOTLOADER)
    endif ()
    pico_add_uf2_output(microboy-nes)
    # debug output
    if (PICO_STDIO_UART)
        pico_enable_stdio_usb(microboy-nes 0)
        pico_enable_stdio_uart(microboy-nes 1)
    else ()
        pico_enable_stdio_usb(microboy-nes 1)
        pico_enable_stdio_uart(microboy-nes 0)
    endif ()
endif ()
