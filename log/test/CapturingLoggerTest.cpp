/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#include "CapturingLogger.h"
#include "se-oss/log/Log.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

namespace {

/**
 * Stand-in for a class under test that takes a const Logger& and keeps its own copy.
 */
class Component
{
public:
    explicit Component(const Logger& logger) : _logger {logger} { }

    void run(int value) { LOG_WARN(_logger, "value %d rejected", value); }

private:
    Logger _logger;
};

}  // namespace

TEST(CapturingLoggerTest, StartsEmpty)
{
    CapturingLogger log;

    EXPECT_EQ(0U, log.count());
    EXPECT_FALSE(log.contains("anything"));
}

TEST(CapturingLoggerTest, RecordsWhatTheClassUnderTestLogs)
{
    CapturingLogger log;
    Component component {log.logger()};

    component.run(5);

    ASSERT_EQ(1U, log.count());
    EXPECT_EQ(LogLevel::WARN, log.sink().records()[0].level);
    EXPECT_THAT(log.sink().records()[0].text, HasSubstr("value 5 rejected"));
}

TEST(CapturingLoggerTest, ContainsMatchesLevelAndSubstring)
{
    CapturingLogger log;

    LOG_INFO(log.logger(), "connected to %s", "host");

    EXPECT_TRUE(log.contains(LogLevel::INFO, "connected to host"));
    EXPECT_TRUE(log.contains(LogLevel::INFO, "connected"));
    EXPECT_TRUE(log.contains("connected to host"));
    EXPECT_FALSE(log.contains(LogLevel::ERROR, "connected to host"));
    EXPECT_FALSE(log.contains(LogLevel::INFO, "disconnected"));
    EXPECT_FALSE(log.contains("disconnected"));
}

TEST(CapturingLoggerTest, UsesTheGivenNameAndAFixedTimestamp)
{
    CapturingLogger log {"Unit", 4U};

    LOG_ERROR(log.logger(), "boom");

    ASSERT_EQ(1U, log.count());
    EXPECT_THAT(log.sink().records()[0].text, HasSubstr("[Unit]"));
    EXPECT_EQ(4U, log.context().contextTag());
    EXPECT_EQ(0U, log.context().time());
}

TEST(CapturingLoggerTest, CopiesOfTheLoggerWriteToTheSameSink)
{
    CapturingLogger log;
    Logger copy = log.logger();
    copy.setName("Copy");

    LOG_DEBUG(copy, "from copy");

    EXPECT_TRUE(log.contains(LogLevel::DEBUG, "[Copy]"));
    EXPECT_TRUE(log.contains(LogLevel::DEBUG, "from copy"));
}

TEST(CapturingLoggerTest, ContextLevelFilters)
{
    CapturingLogger log;
    log.context().setLogFilterLevel(LogLevel::ERROR);

    LOG_INFO(log.logger(), "dropped");
    LOG_ERROR(log.logger(), "kept");

    EXPECT_EQ(1U, log.count());
    EXPECT_FALSE(log.contains("dropped"));
    EXPECT_TRUE(log.contains(LogLevel::ERROR, "kept"));
}

TEST(CapturingLoggerTest, ClearForgetsEverything)
{
    CapturingLogger log;
    LOG_INFO(log.logger(), "before");

    log.clear();

    EXPECT_EQ(0U, log.count());
    EXPECT_FALSE(log.contains("before"));
}
