cmake_minimum_required(VERSION 3.5)

set(PROJECT_NAME 3DViewer-tests)

set(CMAKE_VERBOSE_MAKEFILE ON)

project(${PROJECT_NAME} VERSION 0.1 LANGUAGES C)

# -pthread -lcheck_pic -lrt -lm -lsubunit 

find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED check)

FILE(GLOB FileCheck *.check)
add_custom_command(
        OUTPUT check-tests.c
        COMMAND checkmk ${CMAKE_CURRENT_SOURCE_DIR}/000.include ${FileCheck} > check-tests.c
        COMMENT "generate check-source-file"
        VERBATIM
        )

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")

add_compile_options(
        -Werror
        -Wall
        -Wextra    
        )
        
        set(PROJECT_SOURCES
        check-tests.c
        )
                
include_directories("../kernel")

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE kernel m)

if(MACOS)
        message(STATUS "macos-sistem")
else()
        message(STATUS "unix-sistem")
        target_link_libraries(${PROJECT_NAME} PRIVATE ${CHECK_LIBRARIES})
        target_include_directories(${PROJECT_NAME} PRIVATE ${CHECK_INCLUDE_DIRS})
        target_compile_options(${PROJECT_NAME} PRIVATE ${CHECK_CFLAGS_OTHER})
endif()

# coverage



