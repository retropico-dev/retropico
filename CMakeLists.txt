cmake_minimum_required(VERSION 3.5)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

if (NOT PLATFORM_LINUX)
    include(${CMAKE_SOURCE_DIR}/external/misc/libpico2d/src/platforms/pico/cmake/pico_sdk_import.cmake)
endif ()
add_subdirectory(${CMAKE_SOURCE_DIR}/external/misc/libpico2d)

# pico options
option(PICO_USE_BOOTLOADER "Build bootloader compatible binaries" OFF)

project(microboy)

# TODO: why ?
#[[
set(CMAKE_FLAGS_RELEASE_FIX "-fno-move-loop-invariants -fno-devirtualize -fno-devirtualize-speculatively")
set(CMAKE_C_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_FLAGS_RELEASE_FIX}" CACHE STRING "" FORCE)
set(CMAKE_CXX_FLAGS_RELEASE "-DNDEBUG -O3 ${CMAKE_FLAGS_RELEASE_FIX}" CACHE STRING "" FORCE)
]]

if (NOT PLATFORM_LINUX)
    function(set_linker_script TARGET script)
        target_link_directories(${TARGET} PRIVATE ${CMAKE_SOURCE_DIR}/src/bootloader)
        pico_set_linker_script(${TARGET} ${CMAKE_SOURCE_DIR}/src/bootloader/${script})

        # Add dependencies on the 'included' linker scripts so that the target gets
        # rebuilt if they are changed
        pico_add_link_depend(${TARGET} ${CMAKE_SOURCE_DIR}/src/bootloader/memmap_defines.ld)
        pico_add_link_depend(${TARGET} ${CMAKE_SOURCE_DIR}/src/bootloader/memmap_default.ld)
    endfunction()
else ()
    # i want all executable located at root of build directory
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR} CACHE STRING "" FORCE)
endif ()

# microboy-ui (ui binary)
add_subdirectory(src/ui)

# microboy-nes (infones core)
add_subdirectory(src/cores/InfoNES)

# microboy-gb (Peanut-GB core)
add_subdirectory(src/cores/Peanut-GB)

# microboy-sms (smsplus core)
add_subdirectory(src/cores/smsplus)

# microboy-bootloader
if (PICO_USE_BOOTLOADER)
    add_subdirectory(src/bootloader)
endif ()
