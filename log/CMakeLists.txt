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
        ${CMAKE_CURRENT_LIST_DIR}/src/ConsoleSink.cpp
        ${CMAKE_CURRENT_LIST_DIR}/src/LogContext.cpp
)

target_sources(
        se_oss_log
        PUBLIC
        FILE_SET HEADERS
        TYPE HEADERS
        FILES
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/Conf.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/ILogFilter.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/Log.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/LogRegistry.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/ResourceDatabase.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/Types.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/buffer/AtomicBuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/buffer/IBuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/buffer/ImmediateBuffer.h
        $<$<BOOL:${SE_OSS_SUPPORT_CBOR}>:${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/format/CborFormatter.h>
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/format/PrintfFormatter.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/AggregatedSink.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/BufferSink.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/FilteredSink.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/ILogSink.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/IWriter.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/NullSink.h
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

    include(${CMAKE_SOURCE_DIR}/cmake/static-analysis.cmake)
    se_static_analysis(TARGET se_oss_log)
endif ()

if (${SE_OSS_LOG_BENCHMARKS})
    message(STATUS "[se-log] Benchmarks enabled")
    add_subdirectory(benchmark)
endif ()
