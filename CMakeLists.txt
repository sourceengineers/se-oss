# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.26)
project(se_oss)

# library components
option(SE_OSS_COMPONENT_LOG "Enable the log component" OFF)

# optional dependencies and features
option(SE_OSS_SUPPORT_CBOR "Load support for CBOR binary encoding and decoding" OFF)

option(SE_OSS_EXAMPLES "Compile se-oss examples" OFF)
option(SE_OSS_TESTS "Compile se-oss tests" OFF)
option(SE_OSS_BENCHMARKS "Compile se-oss benchmarks" OFF)

# set component features
# note: these can be overridden by the parent project by forcing the variables
#       into the cache
set(SE_OSS_LOG_EXAMPLES ${SE_OSS_EXAMPLES} CACHE BOOL "")
set(SE_OSS_LOG_TESTS ${SE_OSS_TESTS} CACHE BOOL "")
set(SE_OSS_LOG_BENCHMARKS $SE_OSS_BENCHMARKS} CACHE BOOL "")

add_subdirectory(lib)

if (${SE_OSS_TESTS})
    enable_testing()
    include(cmake/coverage.cmake)
    se_coverage(PROJECT_NAME se_oss)
endif ()

if (${SE_OSS_COMPONENT_LOG})
    message(STATUS "[se-oss] Log component enabled")
    add_subdirectory(log)
endif ()