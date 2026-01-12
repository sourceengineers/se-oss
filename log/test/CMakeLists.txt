# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

enable_testing()

add_executable(
        se_oss_unit_tests
        CborFormatterTest.cpp
)

target_link_libraries(
        se_oss_unit_tests
        PRIVATE
        se_oss::log
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(se_oss_unit_tests)

install(TARGETS se_oss_unit_tests DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT tests)
