// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "LogSinkMock.h"
#include "se-oss/log/LogContext.h"

#include <cstring>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class LogContextTest : public Test, public ITimeProvider
{
protected:
    uint64_t time() const override { return _time; }

    LogSinkMock _sink;
    uint64_t _time {INVALID_TIME};
};

TEST_F(LogContextTest, TimeReturnsProviderTime)
{
    LogContext context(0, "test", _sink, *this);
    _time = 12345U;
    EXPECT_EQ(context.time(), _time);
}

TEST_F(LogContextTest, SetContextTag)
{
    LogContext context(0, "test", _sink, *this);
    EXPECT_EQ(context.contextTag(), 0);
    context.setContextTag(42);
    EXPECT_EQ(context.contextTag(), 42);
}

TEST_F(LogContextTest, Name_ReturnsConstructedName)
{
    LogContext context(0, "mylogger", _sink, *this);
    EXPECT_STREQ(context.name(), "mylogger");
}

TEST_F(LogContextTest, Statistics_InitiallyZero)
{
    LogContext context(0, "test", _sink, *this);
    LogStatistics stats = context.statistics();
    EXPECT_EQ(stats.droppedMessages, 0U);
}

TEST_F(LogContextTest, Statistics_DroppedMessages)
{
    LogContext context(0, "test", _sink, *this);

    // Write with a reserve size larger than buffer capacity.
    std::size_t hugeReserveSize = 1024 * 1024;
    auto region = context.reserveMessage(hugeReserveSize);

    EXPECT_EQ(region.data, nullptr);
    LogStatistics stats = context.statistics();
    EXPECT_GE(stats.droppedMessages, 1U);
}

TEST_F(LogContextTest, DistributeMessages_ImmediateMode_ReturnsEarly)
{
    // The logger is in immediate mode, so distributeMessages will do nothing.
    LogContext context(0, "test", _sink, *this);
    EXPECT_NO_THROW(context.distributeMessages());
}
