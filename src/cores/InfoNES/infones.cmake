cmake_minimum_required(VERSION 3.13)

file(GLOB CORE_SRC
        ${CMAKE_SOURCE_DIR}/external/cores/InfoNES/infones/*.cpp
        )
list(REMOVE_ITEM CORE_SRC
        )

set(CORE_INC
        ${CMAKE_SOURCE_DIR}/external/cores/InfoNES/infones
        ${CMAKE_SOURCE_DIR}/external/cores/InfoNES/pico_lib
        )
if (PLATFORM_LINUX)
    # for pico.h wrapper
    list(APPEND CORE_INC ${CMAKE_SOURCE_DIR}/src/platforms/linux)
endif ()

set(CORE_FLAGS
        )

add_library(infones ${CORE_SRC})
target_include_directories(infones PUBLIC ${CORE_INC})
target_compile_options(infones PUBLIC ${CORE_FLAGS})
if (NOT PLATFORM_LINUX)
    # for pico.h
    target_link_libraries(infones pico_stdlib)
endif ()