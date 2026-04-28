// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "se-oss/log/ILogFilter.h"
#include "se-oss/log/LogFilter.h"

#include <gtest/gtest.h>

using namespace se_oss;

class LogFilterTest : public ::testing::Test
{
};

TEST_F(LogFilterTest, DefaultFilter_PassesAll)
{
    LogFilter filter;
    LogMetadata metadata {};
    metadata.level = LogLevel::TRACE;
    EXPECT_TRUE(filter.passesFilter(metadata));

    metadata.level = LogLevel::FATAL;
    EXPECT_TRUE(filter.passesFilter(metadata));
}

TEST_F(LogFilterTest, SetFilter_Nullptr_RejectAll)
{
    LogFilter filter;
    filter.setFilter(nullptr);

    LogMetadata metadata {};
    metadata.level = LogLevel::INFO;
    EXPECT_FALSE(filter.passesFilter(metadata));
}

TEST_F(LogFilterTest, SetFilter_Custom_Applies)
{
    LogFilter filter;
    filter.setFilter([](const LogMetadata& m) { return m.contextTag == 1; });

    LogMetadata metadata {};
    metadata.level = LogLevel::INFO;
    metadata.contextTag = 1;
    EXPECT_TRUE(filter.passesFilter(metadata));

    metadata.contextTag = 2;
    EXPECT_FALSE(filter.passesFilter(metadata));
}

TEST_F(LogFilterTest, SetLogLevel_CreatesLevelFilter)
{
    LogFilter filter;
    filter.setLogLevel(LogLevel::WARN);

    LogMetadata metadata {};
    metadata.level = LogLevel::DEBUG;
    EXPECT_FALSE(filter.passesFilter(metadata));

    metadata.level = LogLevel::WARN;
    EXPECT_TRUE(filter.passesFilter(metadata));

    metadata.level = LogLevel::FATAL;
    EXPECT_TRUE(filter.passesFilter(metadata));
}
