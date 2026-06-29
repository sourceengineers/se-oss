# Copyright (c) 2025 Source Engineers GmbH
#
# SPDX-License-Identifier: MIT

option(SE_OSS_LOG_EXAMPLE_DEFERRED "Compile se-log benchmarks" OFF)

if (${SE_OSS_LOG_EXAMPLE_DEFERRED})

    add_library(se_oss_log_conf INTERFACE)
    target_include_directories(
            se_oss_log_conf
            INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/deferred_conf
            ${CMAKE_SOURCE_DIR}/log
    )

    add_executable(example_log_printf log_printf.cpp)
    target_compile_definitions(example_log_printf PUBLIC FORMAT_PRINTF)
    target_link_libraries(example_log_printf PRIVATE se_oss::log)

    add_executable(example_log_cbor log_cbor.cpp)
    target_compile_definitions(example_log_cbor PUBLIC FORMAT_CBOR)
    target_link_libraries(example_log_cbor PRIVATE se_oss::log)


    add_executable(example_log_cbor_string_replacement log_cbor_string_replacement.cpp)
    target_compile_definitions(example_log_cbor_string_replacement PUBLIC FORMAT_CBOR_REPLACE_STRINGS)
    target_link_libraries(example_log_cbor_string_replacement PRIVATE se_oss::log)
    include(../cmake/resource_database.cmake)
    se_create_resource_database(TARGET example_log_cbor_string_replacement)

    install(
            TARGETS
            example_log_printf
            example_log_cbor
            example_log_cbor_string_replacement
            DESTINATION examples
            COMPONENT examples
    )

else ()

    add_library(se_oss_log_conf INTERFACE)
    target_include_directories(
            se_oss_log_conf
            INTERFACE
            ${CMAKE_CURRENT_LIST_DIR}/immediate_conf
            ${CMAKE_SOURCE_DIR}/log
    )

    add_executable(example_log_minimal log_minimal.cpp)
    target_link_libraries(example_log_minimal PRIVATE se_oss::log)

    add_executable(example_log_custom log_custom_formatter.cpp)
    target_compile_definitions(example_log_custom PUBLIC FORMAT_CUSTOM)
    target_link_libraries(example_log_custom PRIVATE se_oss::log)

    install(
            TARGETS
            example_log_minimal
            example_log_custom
            DESTINATION examples
            COMPONENT examples
    )
endif ()
