/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#include "CapturingLogSink.h"
#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using namespace se_oss;
using namespace testing;

class CapturingLogSinkTest : public Test
{
protected:
    void SetUp() override
    {
        auto sink = std::make_unique<CapturingLogSink>();
        _sink = sink.get();
        _registry.attachSink(DefaultLogSink::CONSOLE, std::move(sink));
    }

    Logger makeLogger() { return _registry.createLogger(DefaultLogContext::DEFAULT); }

    LogRegistry<> _registry;
    CapturingLogSink* _sink {nullptr};
};

TEST_F(CapturingLogSinkTest, NothingLoggedWhenNothingIsCalled)
{
    Logger logger = makeLogger();
    (void)logger;

    EXPECT_EQ(0U, _sink->count());
    EXPECT_TRUE(_sink->records().empty());
}

TEST_F(CapturingLogSinkTest, RecordsLevelAndMessageText)
{
    Logger logger = makeLogger();

    LOG_WARN(logger, "queue full");

    ASSERT_EQ(1U, _sink->count());
    EXPECT_EQ(LogLevel::WARN, _sink->records()[0].level);
    EXPECT_THAT(_sink->records()[0].text, HasSubstr("queue full"));
}

TEST_F(CapturingLogSinkTest, TextContainsTheFormattedArguments)
{
    Logger logger = makeLogger();

    LOG_INFO(logger, "write to key %d as %s", 42, "blob");

    ASSERT_EQ(1U, _sink->count());
    EXPECT_THAT(_sink->records()[0].text, HasSubstr("write to key 42 as blob"));
}

TEST_F(CapturingLogSinkTest, TextContainsTheLoggerName)
{
    Logger logger = makeLogger();
    logger.setName("DataBroker");

    LOG_ERROR(logger, "dropped");

    ASSERT_EQ(1U, _sink->count());
    EXPECT_THAT(_sink->records()[0].text, HasSubstr("[DataBroker]"));
}

TEST_F(CapturingLogSinkTest, KeepsEveryRecordInOrder)
{
    Logger logger = makeLogger();

    LOG_DEBUG(logger, "first");
    LOG_INFO(logger, "second");
    LOG_ERROR(logger, "third");

    ASSERT_EQ(3U, _sink->count());
    EXPECT_EQ(LogLevel::DEBUG, _sink->records()[0].level);
    EXPECT_THAT(_sink->records()[0].text, HasSubstr("first"));
    EXPECT_EQ(LogLevel::INFO, _sink->records()[1].level);
    EXPECT_THAT(_sink->records()[1].text, HasSubstr("second"));
    EXPECT_EQ(LogLevel::ERROR, _sink->records()[2].level);
    EXPECT_THAT(_sink->records()[2].text, HasSubstr("third"));
}

TEST_F(CapturingLogSinkTest, MessagesBelowTheContextLevelAreNotRecorded)
{
    Logger logger = makeLogger();
    _registry.createOrGetContext(DefaultLogContext::DEFAULT).setLogFilterLevel(LogLevel::ERROR);

    LOG_DEBUG(logger, "not this");
    LOG_INFO(logger, "nor this");
    LOG_ERROR(logger, "but this");

    ASSERT_EQ(1U, _sink->count());
    EXPECT_THAT(_sink->records()[0].text, HasSubstr("but this"));
    EXPECT_THAT(_sink->records()[0].text, Not(HasSubstr("not this")));
}

TEST_F(CapturingLogSinkTest, ClearForgetsEverything)
{
    Logger logger = makeLogger();
    LOG_INFO(logger, "before clear");

    _sink->clear();

    EXPECT_EQ(0U, _sink->count());
}

TEST_F(CapturingLogSinkTest, WorksThroughAPointer)
{
    // The LOG_* macros expand to `logger.log(...)`, and `.` binds tighter than unary `*`,
    // so a dereferenced pointer has to be parenthesised.
    Logger logger = makeLogger();
    Logger* maybeLogger = &logger;

    if (maybeLogger != nullptr) {
        LOG_DEBUG((*maybeLogger), "entering state %s", "Idle");
    }

    ASSERT_EQ(1U, _sink->count());
    EXPECT_THAT(_sink->records()[0].text, HasSubstr("entering state Idle"));
}

TEST_F(CapturingLogSinkTest, CopiedLoggersShareTheSink)
{
    // Loggers are meant to be copied and adjusted locally; every copy still writes through
    // the same context, and therefore the same sink.
    Logger original = makeLogger();
    Logger copy = original;
    copy.setName("copy");

    LOG_INFO(original, "from original");
    LOG_INFO(copy, "from copy");

    ASSERT_EQ(2U, _sink->count());
    EXPECT_THAT(_sink->records()[1].text, HasSubstr("[copy]"));
}

TEST_F(CapturingLogSinkTest, IgnoresRecordsWithoutAValidHeader)
{
    const char garbage[] = "x";

    _sink->write(LogMetadata {}, garbage, sizeof(garbage));

    EXPECT_EQ(0U, _sink->count());
}
