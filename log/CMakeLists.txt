# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

option(SE_OSS_LOG_EXAMPLES "Compile se-log examples" OFF)
option(SE_OSS_LOG_TESTS "Compile se-log tests" OFF)
option(SE_OSS_LOG_BENCHMARKS "Compile se-log benchmarks" OFF)

option(SE_OSS_LOG_REPLACE_STRINGS "Replace format strings with IDs" OFF)
set(SE_OSS_LOG_MAX_LEVEL "TRACE" CACHE STRING "Log levels above here will be included in the output binary")
set(SE_OSS_LOG_RELEASE_MAX_LEVEL "INFO" CACHE STRING "Log levels above here will be included in the output binary")


add_library(
        se_oss_log
        STATIC
        src/sink/ConsoleSink.cpp
)

target_include_directories(
        se_oss_log
        PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include
)

if (${SE_OSS_SUPPORT_CBOR})
    target_link_libraries(
            se_oss_log
            PUBLIC
            tinycbor
    )
endif ()

if (${SE_OSS_LOG_REPLACE_STRINGS})
    message(STATUS "[se-log] Replacing format strings enabled")
    target_compile_definitions(
            se_oss_log
            PUBLIC
            SE_OSS_LOG_REPLACE_STRINGS
    )
endif ()

include(cmake/set_max_log_level.cmake)
se_set_max_log_level(
        TARGET se_oss_log
        MAX_LEVEL ${SE_OSS_LOG_MAX_LEVEL}
        RELEASE_MAX_LEVEL ${SE_OSS_LOG_RELEASE_MAX_LEVEL}
)

add_library(se_oss::log ALIAS se_oss_log)

# examples and tests
if (${SE_OSS_LOG_EXAMPLES})
    message(STATUS "[se-log] Examples enabled")
    add_subdirectory(examples)
endif ()

if (${SE_OSS_LOG_TESTS})
    message(STATUS "[se-log] Tests enabled")
    enable_testing()
    add_subdirectory(test)
endif ()

if (${SE_OSS_LOG_BENCHMARKS})
    message(STATUS "[se-log] Benchmarks enabled")
    add_subdirectory(benchmark)
endif ()
