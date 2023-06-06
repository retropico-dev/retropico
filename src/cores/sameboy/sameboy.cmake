cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/*.c
        )
list(REMOVE_ITEM CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/rewind.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/debugger.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/sm83_disassembler.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/symbol_hash.c
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core/cheats.c
        )

set(CORE_INC
        ${CMAKE_SOURCE_DIR}/src/cores/sameboy
        ${CMAKE_SOURCE_DIR}/external/cores/SameBoy/Core
        )

set(CORE_FLAGS
        -DGB_VERSION="10"
        -DGB_COPYRIGHT_YEAR="2023"
        -DGB_INTERNAL
        -DGB_DISABLE_TIMEKEEPING
        -DGB_DISABLE_REWIND
        -DGB_DISABLE_DEBUGGER
        -DGB_DISABLE_CHEATS
        -Wno-multichar
        )

add_library(sameboy ${CORE_SRC})
target_include_directories(sameboy PUBLIC ${CORE_INC})
target_compile_options(sameboy PUBLIC ${CORE_FLAGS})