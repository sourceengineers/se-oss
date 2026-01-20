# Copyright (c) 2025 Source Engineers GmbH
#
# SPDX-License-Identifier: MIT


add_executable(example_log_minimal log_minimal.cpp)
target_link_libraries(example_log_minimal PRIVATE se_oss::log)


add_executable(example_log_printf log_printf.cpp)
target_link_libraries(example_log_printf PRIVATE se_oss::log)


add_executable(example_log_cbor log_cbor.cpp)
target_link_libraries(example_log_cbor PRIVATE se_oss::log)

add_executable(example_log_custom log_custom_formatter.cpp)
target_link_libraries(example_log_custom PRIVATE se_oss::log)

install(
        TARGETS
        example_log_minimal
        example_log_printf
        example_log_cbor
        DESTINATION examples
        COMPONENT examples
)

if (${SE_OSS_LOG_REPLACE_STRINGS})
    add_executable(example_log_cbor_string_replacement log_cbor_string_replacement.cpp)
    target_link_libraries(example_log_cbor_string_replacement PRIVATE se_oss::log)
    include(../cmake/resource_database.cmake)
    se_create_resource_database(TARGET example_log_cbor_string_replacement)

    install(
            TARGETS example_log_cbor_string_replacement
            DESTINATION examples
            COMPONENT example-string-replacement
    )
endif ()
