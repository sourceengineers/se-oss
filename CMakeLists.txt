# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.26)
project(se_log)


option(SE_LOG_REPLACE_STRINGS "Replace format strings with IDs" OFF)
option(SE_LOG_SUPPORT_CBOR "Support CBOR binary encoding" OFF)
option(SE_LOG_EXAMPLES "Compile se-log examples" OFF)
option(SE_LOG_TESTS "Compile se-log tests" OFF)

if (${PROJECT_IS_TOP_LEVEL})
    enable_language(CXX)
    set(CMAKE_CXX_STANDARD 14)
    set(CMAKE_CXX_EXTENSIONS OFF)

    enable_language(C)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_EXTENSIONS OFF)

    set(SE_LOG_EXAMPLES ON CACHE BOOL "" FORCE)
    set(SE_LOG_TESTS ON CACHE BOOL "" FORCE)
endif ()

add_subdirectory(lib)

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

if (${SE_LOG_SUPPORT_CBOR})
    target_link_libraries(
            se_log
            PUBLIC
            tinycbor
    )
endif ()

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

if (${SE_LOG_TESTS})
    message(STATUS "[se-log] Building Tests")
    enable_testing()
    add_subdirectory(test)
endif ()
