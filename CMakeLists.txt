cmake_minimum_required(VERSION 3.13)

# linux platform
option(PLATFORM_LINUX "Linux target platform" OFF)

if (NOT PLATFORM_LINUX)
    # initialize the SDK based on PICO_SDK_PATH
    # note: this must happen before project()
    include(platforms/pico/cmake/pico_sdk_import.cmake)

    # for hardware_sleep
    include(platforms/pico/cmake/pico_extras_import.cmake)
endif ()

project(microboy)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if (NOT PLATFORM_LINUX)
    # initialize the Raspberry Pi Pico SDK
    pico_sdk_init()
endif ()

# set source files
file(GLOB SOURCES *.c* utility/*.c*)
set(INCLUDES . Peanut-GB utility)

if (PLATFORM_LINUX)
    file(GLOB_RECURSE PLATFORM_SOURCES platforms/linux/*.c*)
    list(APPEND INCLUDES platforms/linux)
    # SDL2
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2)
    list(APPEND INCLUDES ${SDL2_INCLUDE_DIRS})
    list(APPEND PLATFORM_LIBS ${SDL2_LIBRARIES})
else ()
    file(GLOB_RECURSE PLATFORM_SOURCES platforms/pico/*.c*)
    list(APPEND INCLUDES platforms/pico)
    list(APPEND PLATFORM_LIBS pico_stdlib pico_multicore hardware_i2c hardware_sleep)
endif ()

# sources
add_executable(${CMAKE_PROJECT_NAME} ${SOURCES} ${PLATFORM_SOURCES})
# includes
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE ${INCLUDES})
# libs
target_link_libraries(${CMAKE_PROJECT_NAME} ${PLATFORM_LIBS})

if (PLATFORM_LINUX)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE -DLINUX)
else ()
    #target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    #       -DPICO_DEFAULT_UART_TX_PIN=12
    #       -DPICO_DEFAULT_UART_RX_PIN=13)
    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
            PARAM_ASSERTIONS_DISABLE_ALL=1
            PICO_ENTER_USB_BOOT_ON_EXIT=1
            PICO_STDIO_ENABLE_CRLF_SUPPORT=0
            PICO_STDIO_DEFAULT_CRLF=0
            PICO_PRINTF_SUPPORT_FLOAT=0
            PICO_PRINTF_SUPPORT_EXPONENTIAL=0
            PICO_PRINTF_SUPPORT_LONG_LONG=1
            PICO_PRINTF_SUPPORT_PTRDIFF_T=0)

    # run from ram
    pico_set_binary_type(${CMAKE_PROJECT_NAME} copy_to_ram)

    # enable usb output, disable uart output
    pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 1)
    pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 0)

    # create map/bin/hex/uf2 file etc.
    pico_add_extra_outputs(${CMAKE_PROJECT_NAME})
endif ()