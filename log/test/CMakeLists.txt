# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

enable_testing()

add_executable(
        se_log_tests
        CborFormatterTest.cpp
        AtomicBufferTest.cpp
        LogPrintf.cpp
)

target_link_libraries(
        se_log_tests
        PRIVATE
        se_oss::log
        gtest
        gtest_main
)

include(GoogleTest)
gtest_discover_tests(se_log_tests)

install(TARGETS se_log_tests DESTINATION ${CMAKE_INSTALL_PREFIX} COMPONENT tests)
