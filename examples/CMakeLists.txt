# Copyright (c) 2025 Source Engineers GmbH
#
# SPDX-License-Identifier: MIT


add_executable(example_log_printf log_printf.cpp)
target_link_libraries(example_log_printf PRIVATE se::log)


add_executable(example_log_cbor log_cbor.cpp)
target_link_libraries(example_log_cbor PRIVATE se::log)

install(
        TARGETS
        example_log_printf
        example_log_cbor
        DESTINATION ${CMAKE_INSTALL_PREFIX}
        COMPONENT examples
)

if (${SE_LOG_REPLACE_STRINGS})
    add_executable(example_log_cbor_string_replacement log_cbor_string_replacement.cpp)
    target_link_libraries(example_log_cbor_string_replacement PRIVATE se::log)
    include(${CMAKE_SOURCE_DIR}/cmake/resource_database.cmake)
    se_create_resource_database(TARGET example_log_cbor_string_replacement)

    install(
            TARGETS example_log_cbor_string_replacement
            DESTINATION ${CMAKE_INSTALL_PREFIX}
            COMPONENT example-string-replacement
    )
endif ()
