# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

option(SE_OSS_LOG_EXAMPLES "Compile se-log examples" OFF)
option(SE_OSS_LOG_TESTS "Compile se-log tests" OFF)
option(SE_OSS_LOG_BENCHMARKS "Compile se-log benchmarks" OFF)

# user conf
if (NOT TARGET se_oss_log_conf)
    message(STATUS "[se-oss] No conf provided, using default")
    add_library(se_oss_log_conf INTERFACE)
    target_include_directories(se_oss_log_conf INTERFACE ${CMAKE_CURRENT_LIST_DIR}/default_conf)
endif ()

# log library
add_library(
        se_oss_log
        STATIC
        ${CMAKE_CURRENT_LIST_DIR}/src/ConsoleWriter.cpp
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
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/format/StringBuffer.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/AggregatedSink.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/BufferSink.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/ConsoleSink.h
        ${CMAKE_CURRENT_LIST_DIR}/include/se-oss/log/sink/ConsoleWriter.h
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

target_link_libraries(
        se_oss_log
        PUBLIC
        se_oss_log_conf
)

if (${SE_OSS_SUPPORT_CBOR})
    target_link_libraries(
            se_oss_log
            PUBLIC
            tinycbor
    )
endif ()

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
