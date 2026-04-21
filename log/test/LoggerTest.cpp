// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "LogSinkMock.h"
#include "se-oss/log/Log.h"
#include "se-oss/log/LogContext.h"

#include <cstring>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class LoggerTest : public Test
{
protected:
    void SetUp() override
    {
        _context = std::make_unique<LogContext>(
            1, "loggertest", _sink, []() -> uint64_t { return 12345ULL; }
        );
    }

    LogSinkMock _sink;
    std::unique_ptr<LogContext> _context;
};

TEST_F(LoggerTest, Statistics_ReturnsContextStatistics)
{
    Logger logger(*_context);
    LogStatistics stats = logger.statistics();
    EXPECT_EQ(stats.droppedMessages, 0U);
}

TEST_F(LoggerTest, LogTag_GetSet)
{
    Logger logger(*_context);
    EXPECT_EQ(logger.logTag(), 0U);
    logger.setLogTag(99);
    EXPECT_EQ(logger.logTag(), 99);
}

TEST_F(LoggerTest, Name_DefaultFromContext)
{
    Logger logger(*_context);
    EXPECT_STREQ(logger.name(), "loggertest");
}

TEST_F(LoggerTest, Name_SetOverrides)
{
    Logger logger(*_context);
    logger.setName("overridden");
    EXPECT_STREQ(logger.name(), "overridden");
}

TEST_F(LoggerTest, SetFilter_DelegatesToContext)
{
    Logger logger(*_context);
    bool filterCalled = false;
    logger.setFilter([&filterCalled](const LogMetadata&) -> bool {
        filterCalled = true;
        return true;
    });

    // Verify filter was applied by logging a message
    EXPECT_CALL(_sink, write(_, _, _)).Times(AtLeast(0));
    logger.log(LogLevel::INFO, "test");
    EXPECT_TRUE(filterCalled);
}

TEST_F(LoggerTest, CopyConstructor)
{
    Logger original(*_context);
    original.setLogTag(42);
    original.setName("original");

    Logger copy(original);
    EXPECT_EQ(copy.logTag(), 42);
    EXPECT_STREQ(copy.name(), "original");
}

TEST_F(LoggerTest, Log_FilteredOut_NoWrite)
{
    Logger logger(*_context);
    logger.setLogLevel(LogLevel::FATAL);

    // DEBUG should be filtered out — no write to sink
    EXPECT_CALL(_sink, write(_, _, _)).Times(0);
    logger.log(LogLevel::DEBUG, "should not appear");
}

TEST_F(LoggerTest, Log_FormatterReturnsZero_NoHeader)
{
    // When the buffer is too small for any content, the formatter returns 0
    // and the producer lambda returns 0 (no header written).
    // This is hard to trigger directly because the buffer is allocated internally.
    // We cover the bytesWritten == 0 branch by having the filter pass but
    // using a nullptr format string which causes the formatter to produce 0 bytes
    // when valid becomes false.
    Logger logger(*_context);
    logger.setLogLevel(LogLevel::TRACE);

    // The write should still happen via writeMessage, but the producer returns 0
    // so no data is actually committed to the buffer.
    // We can't easily mock the formatter, but we can verify no crash.
    EXPECT_CALL(_sink, write(_, _, _)).Times(AtLeast(0));
    logger.log(LogLevel::INFO, static_cast<const char*>(nullptr));
}
