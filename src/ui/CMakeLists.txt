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
add_executable(${PROJECT_NAME} ${UI_SOURCES})
target_add_resources(${PROJECT_NAME} ${UI_RESOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC ${UI_INCLUDES})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${UI_FLAGS})
target_link_libraries(${PROJECT_NAME} ${UI_LIBS} retropico-common pico2d-skeleton)

if (NOT PLATFORM_LINUX)
    if (PICO_USE_BOOTLOADER)
        set_linker_script(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/src/bootloader/memmap_ui.ld)
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${UI_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        set_linker_flash_size_16(${PROJECT_NAME})
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
