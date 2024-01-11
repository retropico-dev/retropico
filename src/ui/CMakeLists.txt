cmake_minimum_required(VERSION 3.5)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(microboy-ui)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

# set source files
file(GLOB UI_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.c*)
set(UI_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR})
file(GLOB UI_RESOURCES romfs/*)
set(UI_FLAGS -DMB_UI)

####################
# skeleton library
####################
#add_executable(microboy-ui ${UI_SOURCES} ${RES})
add_executable(microboy-ui ${UI_SOURCES})
target_add_resources(microboy-ui ${UI_RESOURCES})
target_include_directories(microboy-ui PUBLIC ${UI_INCLUDES})
target_compile_definitions(microboy-ui PUBLIC ${UI_FLAGS})
target_link_libraries(microboy-ui ${UI_LIBS} pico2d-skeleton)

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        set_linker_script(microboy-ui memmap_ui.ld)
        target_compile_definitions(microboy-ui PUBLIC ${UI_FLAGS} PICO_USE_BOOTLOADER)
    endif ()
    pico_add_uf2_output(microboy-ui)
    # debug output
    if (PICO_STDIO_UART)
        pico_enable_stdio_usb(microboy-ui 0)
        pico_enable_stdio_uart(microboy-ui 1)
    else ()
        pico_enable_stdio_usb(microboy-ui 1)
        pico_enable_stdio_uart(microboy-ui 0)
    endif ()
endif ()
