cmake_minimum_required(VERSION 3.5)

project(kernel VERSION 0.1 LANGUAGES C)

# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")

add_compile_options(
        -Werror
        -Wall
        -Wextra        
)

set(SOURCES
        dataset.c
        transform.c
        utils.c
)

set(HEADERS 
        dataset.h
        transform.h
        utils.h
)

add_library(kernel STATIC ${HEADERS} ${SOURCES})
