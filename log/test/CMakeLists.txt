# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

enable_testing()

add_executable(
        se_log_tests
        AtomicBufferTest.cpp
        BufferSinkTest.cpp
        CborFormatterTest.cpp
        ConsoleSinkTest.cpp
        FilteredSinkTest.cpp
        LogPrintfTest.cpp
        NoBufferTest.cpp
)

target_link_libraries(
        se_log_tests
        PRIVATE
        se_oss::log
        GTest::gmock
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(se_log_tests)

install(TARGETS se_log_tests DESTINATION test COMPONENT tests)
