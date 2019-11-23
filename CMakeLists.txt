cmake_minimum_required(VERSION 2.6)

project(ideal LANGUAGES CXX)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

option(BUILD_EXAMPLES "Build Ideal examples" ON)

set(CMAKE_CXX_FLAGS "-g -Wall -std=c++11")
set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE)
message(STATUS "CXX_FLAGS = " ${CMAKE_CXX_FLAGS} " " ${CMAKE_CXX_FLAGS_${BUILD_TYPE}})

find_library(GOOGLETEST_LIBRARY NAMES gtest)

include_directories(${PROJECT_SOURCE_DIR})

add_subdirectory(${PROJECT_SOURCE_DIR}/ideal/base)
add_subdirectory(${PROJECT_SOURCE_DIR}/ideal/net)

if(BUILD_EXAMPLES)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example/base)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example/net)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example/filetransfer)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example/maxconnection)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example/pingpong)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example/http)
    add_subdirectory(${PROJECT_SOURCE_DIR}/example/sudoku)
endif()

