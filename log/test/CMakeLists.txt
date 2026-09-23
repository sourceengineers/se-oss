# Copyright (c) 2025 Source Engineers GmbH, Switzerland
# Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
# SPDX-License-Identifier: MIT

# header-only test support (CapturingLogSink, CapturingLogger, LogSinkMock) for the unit tests
# of projects using the logger; always available, independent of SE_OSS_LOG_TESTS
add_library(se_oss_log_test_support INTERFACE)
target_include_directories(se_oss_log_test_support INTERFACE ${CMAKE_CURRENT_LIST_DIR})
target_link_libraries(se_oss_log_test_support INTERFACE se_oss::log)
add_library(se_oss::log_test_support ALIAS se_oss_log_test_support)

if (NOT ${SE_OSS_LOG_TESTS})
    return()
endif ()

# tests of the log library
enable_testing()

add_executable(
        se_log_tests
        AggregatedSinkTest.cpp
        AtomicBufferTest.cpp
        BufferSinkTest.cpp
        CapturingLoggerTest.cpp
        CapturingLogSinkTest.cpp
        CborFormatterTest.cpp
        ConsoleWriterTest.cpp
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
        se_oss::log_test_support
        GTest::gmock
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(se_log_tests)

install(TARGETS se_log_tests DESTINATION test COMPONENT tests)
