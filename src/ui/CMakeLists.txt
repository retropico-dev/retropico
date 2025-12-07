cmake_minimum_required(VERSION 3.10)
#set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "ON")

project(retropico-ui)
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
        retropico_setup_ld(${PROJECT_NAME} "${CMAKE_SOURCE_DIR}/src/bootloader/memmap_ui.ld")
        target_compile_definitions(${PROJECT_NAME} PUBLIC ${GB_FLAGS} PICO_USE_BOOTLOADER)
    else ()
        target_set_pico2d_stuff(${PROJECT_NAME})
    endif ()
endif ()
