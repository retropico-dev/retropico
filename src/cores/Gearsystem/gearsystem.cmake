cmake_minimum_required(VERSION 3.13)

file(GLOB_RECURSE CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/Gearsystem/src/*.c*
        )
list(REMOVE_ITEM CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/Gearsystem/src/miniz/miniz.c
        )

set(CORE_INC
        ${CMAKE_SOURCE_DIR}/external/cores/Gearsystem/src
        )

set(CORE_FLAGS
        -DMINIZ_NO_TIME
        -DMINIZ_NO_ARCHIVE_WRITING_APIS
        -DMINIZ_NO_ZLIB_APIS
        -DGEARSYSTEM_DISABLE_DISASSEMBLER
        -DDEBUG_GEARSYSTEM
        )

add_library(gearsystem ${CORE_SRC})
target_include_directories(gearsystem PUBLIC ${CORE_INC})
target_compile_options(gearsystem PUBLIC ${CORE_FLAGS})