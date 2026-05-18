# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

enable_testing()

add_executable(
        se_log_tests
        AggregatedSinkTest.cpp
        AtomicBufferTest.cpp
        BufferSinkTest.cpp
        CborFormatterTest.cpp
        ConsoleSinkTest.cpp
        FilteredSinkTest.cpp
        ImmediateBufferTest.cpp
        LogContextTest.cpp
        LogFilterTest.cpp
        LoggerTest.cpp
        LogHeaderTest.cpp
        LogPrintfTest.cpp
        LogRegistryTest.cpp
        NullSinkTest.cpp
        PrintfFormatterTest.cpp
        StringBufferTest.cpp
        TypesTest.cpp
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
