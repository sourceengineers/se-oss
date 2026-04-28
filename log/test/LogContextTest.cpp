// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "LogSinkMock.h"
#include "se-oss/log/LogContext.h"

#include <cstring>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class LogContextTest : public Test
{
protected:
    LogSinkMock _sink;
};

TEST_F(LogContextTest, TimeReturnsInvalidWhenNoProvider)
{
    LogContext context(0, "test", _sink, nullptr);
    EXPECT_EQ(context.time(), INVALID_TIME);
}

TEST_F(LogContextTest, SetContextTag)
{
    LogContext context(0, "test", _sink, nullptr);
    EXPECT_EQ(context.contextTag(), 0);
    context.setContextTag(42);
    EXPECT_EQ(context.contextTag(), 42);
}

TEST_F(LogContextTest, Name_ReturnsConstructedName)
{
    LogContext context(0, "mylogger", _sink, nullptr);
    EXPECT_STREQ(context.name(), "mylogger");
}

TEST_F(LogContextTest, Statistics_InitiallyZero)
{
    LogContext context(0, "test", _sink, nullptr);
    LogStatistics stats = context.statistics();
    EXPECT_EQ(stats.droppedMessages, 0U);
}

TEST_F(LogContextTest, Statistics_DroppedMessages)
{
    LogContext context(0, "test", _sink, nullptr);

    // Write with a reserve size larger than buffer capacity.
    std::size_t hugeReserveSize = 1024 * 1024;
    context.writeMessage(hugeReserveSize, [](void*, std::size_t) -> std::size_t { return 0; });

    LogStatistics stats = context.statistics();
    EXPECT_GE(stats.droppedMessages, 1U);
}

TEST_F(LogContextTest, DistributeMessages_ImmediateMode_ReturnsEarly)
{
    // The logger is in immediate mode, so distributeMessages will do nothing.
    LogContext context(0, "test", _sink, nullptr);
    EXPECT_NO_THROW(context.distributeMessages());
}
