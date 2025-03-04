cmake_minimum_required(VERSION 3.5)

project(Project)

# set(CMAKE_VERBOSE_MAKEFILE ON)

# Options

option(PROJECT_TESTING "Включить модульное тестирование" OFF)
option(PROJECT_COVERAGE "Включить измерение покрытия кода тестами" OFF)
option(PROJECT_DOC "Включить документирование" OFF)

add_subdirectory(kernel)
add_subdirectory(app)

if(NOT PROJECT_TESTING)
    message(STATUS "Тестирование проекта выключено")
else()
    add_subdirectory(tests)
endif()

if(NOT PROJECT_DOC)
    message(STATUS "Автодокументация проекта выключена")
else()
    add_subdirectory(doc)
endif()