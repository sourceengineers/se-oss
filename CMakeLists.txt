# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.26)
project(se_oss)


option(SE_LOG_REPLACE_STRINGS "Replace format strings with IDs" OFF)
option(SE_LOG_SUPPORT_CBOR "Support CBOR binary encoding" OFF)
option(SE_LOG_EXAMPLES "Compile se-log examples" OFF)
option(SE_LOG_TESTS "Compile se-log tests" OFF)

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
