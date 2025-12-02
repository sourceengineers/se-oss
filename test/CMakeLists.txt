# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

# todo :)

enable_testing()

include(FetchContent)
FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG        v1.17.0
)
FetchContent_MakeAvailable(googletest)


add_executable(
        unit_tests
        CborFormatterTest.cpp
)

target_link_libraries(
        unit_tests
        PRIVATE
        se::log
        GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(unit_tests)

