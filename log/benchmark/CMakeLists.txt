# Copyright (c) 2025 Source Engineers GmbH, Switzerland
# Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
# SPDX-License-Identifier: MIT

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.4
)
FetchContent_MakeAvailable(benchmark)

# The benchmarks bring their own log configuration (deferred logging, see conf/UserLogConf.h) unless
# the build already has one, e.g. from the examples. The formatter variants further down rely on
# that configuration honouring the FORMAT_* definitions, so they are only offered when it is ours.
if (NOT TARGET se_oss_log_conf)
    add_library(se_oss_log_conf INTERFACE)
    target_include_directories(se_oss_log_conf INTERFACE ${CMAKE_CURRENT_LIST_DIR}/conf)
    set(SE_OSS_LOG_BENCHMARK_OWNS_CONF ON)
else ()
    set(SE_OSS_LOG_BENCHMARK_OWNS_CONF OFF)
    message(STATUS "[se-log] Benchmarks use the log configuration already present in the build")
endif ()

add_executable(se_log_benchmark LogBenchmark.cpp)
target_link_libraries(
        se_log_benchmark
        PRIVATE
        se_oss::log
        benchmark
)

if (SE_OSS_LOG_BENCHMARK_OWNS_CONF AND SE_OSS_SUPPORT_CBOR)
    add_executable(se_log_benchmark_cbor LogBenchmark.cpp)
    target_compile_definitions(se_log_benchmark_cbor PRIVATE FORMAT_CBOR)
    target_link_libraries(
            se_log_benchmark_cbor
            PRIVATE
            se_oss::log
            benchmark
    )

    add_executable(se_log_benchmark_lookup LogBenchmark.cpp)
    target_compile_definitions(se_log_benchmark_lookup PRIVATE FORMAT_CBOR_REPLACE_STRINGS)
    target_link_libraries(
            se_log_benchmark_lookup
            PRIVATE
            se_oss::log
            benchmark
    )
    include(${CMAKE_CURRENT_LIST_DIR}/../cmake/resource_database.cmake)
    se_create_resource_database(TARGET se_log_benchmark_lookup)
endif ()
