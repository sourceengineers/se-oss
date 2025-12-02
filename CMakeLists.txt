# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.26)
project(se_log)


option(SE_LOG_REPLACE_STRINGS "Replace format strings with IDs" OFF)


if (${PROJECT_IS_TOP_LEVEL})
    enable_language(CXX)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_EXTENSIONS OFF)

    enable_language(C)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_EXTENSIONS OFF)

    set(SE_LOG_EXAMPLES ON CACHE BOOL "" FORCE)
    set(SE_LOG_TEST ON CACHE BOOL "" FORCE)
endif ()

# todo: move to /lib
include(FetchContent)
set(WITH_FLOATING_POINT ON CACHE BOOL "" FORCE)
set(WITH_CBOR2JSON OFF CACHE BOOL "" FORCE)
set(WITH_TOOLS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        tinycbor
        GIT_REPOSITORY https://github.com/intel/tinycbor.git
        GIT_TAG 17362494e2ddc567ba5ccba7b4e16f0639f578a3 # need to use main, because cmake version is no release yet
)
FetchContent_MakeAvailable(tinycbor)



add_library(
        se_log
        STATIC
        src/sink/ConsoleSink.cpp
)

target_include_directories(
        se_log
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

target_link_libraries(
        se_log
        PUBLIC
        tinycbor
)

if (${SE_LOG_REPLACE_STRINGS})
    message(STATUS "[se-log] Replacing format strings enabled")
    target_compile_definitions(
            se_log
            PUBLIC
            SE_LOG_REPLACE_STRINGS
    )
endif ()

add_library(se::log ALIAS se_log)

# examples and tests
if (${SE_LOG_EXAMPLES})
    message(STATUS "[se-log] Building Examples")
    add_subdirectory(examples)
endif ()

if (${SE_LOG_TEST})
    message(STATUS "[se-log] Building Tests")
    enable_testing()
    add_subdirectory(test)
endif ()
