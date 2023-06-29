cmake_minimum_required(VERSION 3.13)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(microboy-gb)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# set source files
file(GLOB GB_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/*.c*
        ${CMAKE_SOURCE_DIR}/external/cores/Peanut-GB/examples/sdl2/minigb_apu/*.c*
        )
set(GB_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_SOURCE_DIR}/external/cores/Peanut-GB
        ${CMAKE_SOURCE_DIR}/external/cores/Peanut-GB/examples/sdl2/minigb_apu
        )
set(GB_FLAGS -DMB_GB)

####################
# Peanut-GB binary
####################
add_executable(microboy-gb ${GB_SOURCES})
target_include_directories(microboy-gb PUBLIC ${GB_INCLUDES})
target_compile_definitions(microboy-gb PUBLIC ${GB_FLAGS})
target_link_libraries(microboy-gb ${GB_LIBS} microboy-skeleton)
if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        set_linker_script(microboy-gb memmap_gb.ld)
        target_compile_definitions(microboy-gb PUBLIC ${GB_FLAGS} PICO_USE_BOOTLOADER)
    endif ()
    pico_add_uf2_output(microboy-gb)
    # debug output
    if (PICO_STDIO_UART)
        pico_enable_stdio_usb(microboy-gb 0)
        pico_enable_stdio_uart(microboy-gb 1)
    else ()
        pico_enable_stdio_usb(microboy-gb 1)
        pico_enable_stdio_uart(microboy-gb 0)
    endif ()
endif ()
