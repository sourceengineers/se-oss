// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "se-oss/log/Types.h"

#include <gtest/gtest.h>

using namespace se_oss;

TEST(Types, ToString_AllLevels)
{
    EXPECT_STREQ(toString(LogLevel::TRACE), "trace");
    EXPECT_STREQ(toString(LogLevel::DEBUG), "debug");
    EXPECT_STREQ(toString(LogLevel::INFO), "info");
    EXPECT_STREQ(toString(LogLevel::WARN), "warn");
    EXPECT_STREQ(toString(LogLevel::ERROR), "error");
    EXPECT_STREQ(toString(LogLevel::FATAL), "fatal");
    EXPECT_STREQ(toString(LogLevel::OFF), "off");
    // Default branch: invalid level
    EXPECT_STREQ(toString(static_cast<LogLevel>(100)), "");
}

TEST(Types, ToShortString_AllLevels)
{
    EXPECT_STREQ(toShortString(LogLevel::TRACE), "T");
    EXPECT_STREQ(toShortString(LogLevel::DEBUG), "D");
    EXPECT_STREQ(toShortString(LogLevel::INFO), "I");
    EXPECT_STREQ(toShortString(LogLevel::WARN), "W");
    EXPECT_STREQ(toShortString(LogLevel::ERROR), "E");
    EXPECT_STREQ(toShortString(LogLevel::FATAL), "F");
    EXPECT_STREQ(toShortString(LogLevel::OFF), "O");
    EXPECT_STREQ(toShortString(static_cast<LogLevel>(100)), "");
}

TEST(Types, ToUint_AllLevels)
{
    EXPECT_EQ(toUint(LogLevel::TRACE), 0);
    EXPECT_EQ(toUint(LogLevel::DEBUG), 1);
    EXPECT_EQ(toUint(LogLevel::INFO), 2);
    EXPECT_EQ(toUint(LogLevel::WARN), 3);
    EXPECT_EQ(toUint(LogLevel::ERROR), 4);
    EXPECT_EQ(toUint(LogLevel::FATAL), 5);
    EXPECT_EQ(toUint(LogLevel::OFF), UINT8_MAX);
}

TEST(Types, ToLogLevel_AllValues)
{
    EXPECT_EQ(toLogLevel(0), LogLevel::TRACE);
    EXPECT_EQ(toLogLevel(1), LogLevel::DEBUG);
    EXPECT_EQ(toLogLevel(2), LogLevel::INFO);
    EXPECT_EQ(toLogLevel(3), LogLevel::WARN);
    EXPECT_EQ(toLogLevel(4), LogLevel::ERROR);
    EXPECT_EQ(toLogLevel(5), LogLevel::FATAL);
    EXPECT_EQ(toLogLevel(UINT8_MAX), LogLevel::OFF);
    // Default branch: unknown value maps to OFF
    EXPECT_EQ(toLogLevel(42), LogLevel::OFF);
}
