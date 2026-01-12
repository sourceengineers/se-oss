# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.26)
project(se_oss)


option(SE_LOG_REPLACE_STRINGS "Replace format strings with IDs" OFF)
option(SE_LOG_SUPPORT_CBOR "Support CBOR binary encoding" OFF)
option(SE_LOG_EXAMPLES "Compile se-log examples" OFF)
option(SE_LOG_TESTS "Compile se-log tests" OFF)
option(SE_LOG_BENCHMARKS "Compile se-log benchmarks" OFF)
set(SE_LOG_MAX_LEVEL "TRACE" CACHE STRING "Log levels above here will be included in the output binary")
set(SE_LOG_RELEASE_MAX_LEVEL "INFO" CACHE STRING "Log levels above here will be included in the output binary")

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

include(cmake/set_max_log_level.cmake)
se_set_max_log_level(
        TARGET se_log
        MAX_LEVEL ${SE_LOG_MAX_LEVEL}
        RELEASE_MAX_LEVEL ${SE_LOG_RELEASE_MAX_LEVEL}
)

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

if (${SE_LOG_BENCHMARKS})
    message(STATUS "[se-log] Building Benchmarks")
    add_subdirectory(benchmark)
endif ()
