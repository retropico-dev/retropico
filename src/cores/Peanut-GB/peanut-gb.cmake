cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/Peanut-GB/examples/sdl2/minigb_apu/*.c*
        )

set(CORE_INC
        ${CMAKE_SOURCE_DIR}/external/cores/Peanut-GB
        ${CMAKE_SOURCE_DIR}/external/cores/Peanut-GB/examples/sdl2/minigb_apu
        )

set(CORE_FLAGS)

add_library(peanut-gb ${CORE_SRC})
target_include_directories(peanut-gb PUBLIC ${CORE_INC})
target_compile_definitions(peanut-gb PUBLIC ${CORE_FLAGS})