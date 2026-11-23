# Copyright (c) 2025 Source Engineers GmbH, Switzerland
# Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
# SPDX-License-Identifier: MIT

# header-only test support of projects using the logger; always available, independent of SE_OSS_LOG_TESTS
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

# the tests log through the macros with the compile-time format check on
target_compile_definitions(se_log_tests PRIVATE SE_OSS_LOG_CHECK_PRINTF_FORMAT)

# compile-time format check of the LOG_* macros (GCC and Clang): the tests compile with mismatches as errors, and the
# same mismatched call fails to compile with SE_OSS_LOG_CHECK_PRINTF_FORMAT and compiles without it (the default)
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(se_log_tests PRIVATE -Werror=format)

    add_library(se_log_format_check_on OBJECT EXCLUDE_FROM_ALL FormatCheckMismatch.cpp)
    target_link_libraries(se_log_format_check_on PRIVATE se_oss::log)
    target_compile_options(se_log_format_check_on PRIVATE -Werror=format)
    target_compile_definitions(se_log_format_check_on PRIVATE SE_OSS_LOG_CHECK_PRINTF_FORMAT)

    add_library(se_log_format_check_off OBJECT EXCLUDE_FROM_ALL FormatCheckMismatch.cpp)
    target_link_libraries(se_log_format_check_off PRIVATE se_oss::log)
    target_compile_options(se_log_format_check_off PRIVATE -Werror=format)

    add_test(
            NAME LogFormatCheck.MismatchFailsToCompileWithTheCheck
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target se_log_format_check_on
    )
    # the diagnostic itself (GCC "error: format '%s' expects ...", Clang "error: format specifies ..."), not the failed
    # command line, which also holds -Werror=format
    set_tests_properties(
            LogFormatCheck.MismatchFailsToCompileWithTheCheck
            PROPERTIES PASS_REGULAR_EXPRESSION "error: format" RESOURCE_LOCK se_log_build_dir
    )

    add_test(
            NAME LogFormatCheck.MismatchCompilesByDefault
            COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target se_log_format_check_off
    )
    set_tests_properties(LogFormatCheck.MismatchCompilesByDefault PROPERTIES RESOURCE_LOCK se_log_build_dir)
endif ()
