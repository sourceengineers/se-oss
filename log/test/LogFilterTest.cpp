// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "se-oss/log/filter/LogFilter.h"

#include <gtest/gtest.h>

using namespace se_oss;

namespace {

class ContextTagFilter final : public ILogFilter
{
public:
    explicit ContextTagFilter(uint8_t contextTag) : _contextTag {contextTag} { }

    bool passesFilter(const LogMetadata& metadata) const override { return metadata.contextTag == _contextTag; }

private:
    uint8_t _contextTag;
};

}  // namespace

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

TEST_F(LogFilterTest, NullCustomFilter_PassesLevelFilter)
{
    LogFilter filter;
    filter.setCustomLogFilter(nullptr);

    LogMetadata metadata {};
    metadata.level = LogLevel::INFO;
    EXPECT_TRUE(filter.passesFilter(metadata));
}

TEST_F(LogFilterTest, CustomFilter_Applies)
{
    LogFilter filter;
    ContextTagFilter contextTagFilter {1};
    filter.setCustomLogFilter(&contextTagFilter);

    LogMetadata metadata {};
    metadata.level = LogLevel::INFO;
    metadata.contextTag = 1;
    EXPECT_TRUE(filter.passesFilter(metadata));

    metadata.contextTag = 2;
    EXPECT_FALSE(filter.passesFilter(metadata));
}

TEST_F(LogFilterTest, LogLevelFilter_Applies)
{
    LogFilter filter;
    filter.setLogFilterLevel(LogLevel::WARN);

    LogMetadata metadata {};
    metadata.level = LogLevel::DEBUG;
    EXPECT_FALSE(filter.passesFilter(metadata));

    metadata.level = LogLevel::WARN;
    EXPECT_TRUE(filter.passesFilter(metadata));

    metadata.level = LogLevel::FATAL;
    EXPECT_TRUE(filter.passesFilter(metadata));
}

TEST_F(LogFilterTest, LogLevelAndCustomFilter_MustBothPass)
{
    LogFilter filter;
    ContextTagFilter contextTagFilter {1};
    filter.setLogFilterLevel(LogLevel::WARN);
    filter.setCustomLogFilter(&contextTagFilter);

    LogMetadata metadata {};
    metadata.level = LogLevel::INFO;
    metadata.contextTag = 1;
    EXPECT_FALSE(filter.passesFilter(metadata));

    metadata.level = LogLevel::WARN;
    metadata.contextTag = 2;
    EXPECT_FALSE(filter.passesFilter(metadata));

    metadata.contextTag = 1;
    EXPECT_TRUE(filter.passesFilter(metadata));
}
