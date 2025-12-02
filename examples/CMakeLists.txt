# Copyright (c) 2025 Source Engineers GmbH
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.31)

add_executable(
        example_console_printf
        console_printf.cpp
)

#target_compile_options(example_console_printf PRIVATE -O2)

target_link_options(
        example_console_printf
        PUBLIC
        LINKER:-Map=example_console_printf.map
)

#target_compile_options(se_log_example PRIVATE -fconstexpr-depth=16000)

target_link_libraries(
        example_console_printf
        PRIVATE
        se::log
)

include(${CMAKE_SOURCE_DIR}/cmake/log_database.cmake)
se_create_log_database(TARGET example_console_printf)

add_executable(
        example_console_binary
        console_cbor.cpp
)

target_link_libraries(
        example_console_binary
        PRIVATE
        se::log
)

