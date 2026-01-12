# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
FetchContent_Declare(
        benchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG v1.9.4
)
FetchContent_MakeAvailable(benchmark)

add_executable(benchmark_log_printf LogBenchmark.cpp)
target_compile_definitions(benchmark_log_printf PRIVATE BENCH_PRINTF)
target_link_libraries(
        benchmark_log_printf
        PRIVATE
        se::log
        benchmark
)

add_executable(benchmark_log_cbor LogBenchmark.cpp)
target_compile_definitions(benchmark_log_cbor PRIVATE BENCH_PRINTF)
target_link_libraries(
        benchmark_log_cbor
        PRIVATE
        se::log
        benchmark
)

add_executable(benchmark_log_lookup LogBenchmark.cpp)
target_compile_definitions(benchmark_log_lookup PRIVATE BENCH_CBOR)
target_link_libraries(
        benchmark_log_lookup
        PRIVATE
        se::log
        benchmark
)
include(${CMAKE_SOURCE_DIR}/cmake/resource_database.cmake)
se_create_resource_database(TARGET benchmark_log_lookup)
