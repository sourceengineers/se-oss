/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/format/PrintfFormatter.h"

#include <gtest/gtest.h>

#include <cstring>
#include <string>

using namespace se_oss;

TEST(PrintfFormatter, FormatBasicMessage)
{
    char buf[256] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::INFO;
    record.loggerName = "src";
    record.timestamp = 0;

    std::size_t len = PrintfFormatter<TimeFormat::NONE>::format(buf, sizeof(buf), record, "hello");
    std::string output(buf, len);
    EXPECT_EQ(output, "I [src] -- hello\n");
}

TEST(PrintfFormatter, FormatWithArgs)
{
    char buf[256] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::DEBUG;
    record.loggerName = "app";
    record.timestamp = 0;

    std::size_t len = PrintfFormatter<TimeFormat::NONE>::format(buf, sizeof(buf), record, "%d %s", 42, "test");
    std::string output(buf, len);
    EXPECT_EQ(output, "D [app] -- 42 test\n");
}

TEST(PrintfFormatter, FormatWithHex8Time)
{
    char buf[256] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::INFO;
    record.loggerName = "src";
    record.timestamp = 0xDEADBEEFULL;

    std::size_t len = PrintfFormatter<TimeFormat::HEX_8>::format(buf, sizeof(buf), record, "msg");
    std::string output(buf, len);
    EXPECT_EQ(output, "DEADBEEF I [src] -- msg\n");
}

TEST(PrintfFormatter, FormatWithDecimalTime)
{
    char buf[256] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::WARN;
    record.loggerName = "src";
    record.timestamp = 99999;

    std::size_t len = PrintfFormatter<TimeFormat::DECIMAL>::format(buf, sizeof(buf), record, "warn");
    std::string output(buf, len);
    EXPECT_EQ(output, "99999 W [src] -- warn\n");
}

TEST(PrintfFormatter, FormatWithISO8601Time)
{
    char buf[256] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::ERROR;
    record.loggerName = "src";
    record.timestamp = 1737366731209138ULL;

    std::size_t len = PrintfFormatter<TimeFormat::ISO8601>::format(buf, sizeof(buf), record, "err");
    std::string output(buf, len);
    EXPECT_EQ(output, "2025-01-20T09:52:11.209Z E [src] -- err\n");
}

TEST(PrintfFormatter, FormatTruncation)
{
    // Very small buffer — not enough for the full message
    char buf[32] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::INFO;
    record.loggerName = "src";
    record.timestamp = 0;

    std::size_t len = PrintfFormatter<TimeFormat::NONE>::format(buf, sizeof(buf), record, "hello world this is too long %u", 42U);
    std::string output(buf, len);

    EXPECT_EQ(len, 31);
    EXPECT_EQ(output, "I [src] -- hello world this is ");
}

TEST(PrintfFormatter, InvalidFormat)
{
    char buf[256] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::ERROR;
    record.loggerName = "src";
    record.timestamp = 1737366731209138ULL;

    std::size_t len = PrintfFormatter<TimeFormat::ISO8601>::format(buf, sizeof(buf), record, "invalid %z", 42U);
    std::string output(buf, len);
    EXPECT_EQ(len, 0U);
}

TEST(PrintfFormatter, FormatAllLogLevels)
{
    const LogLevel levels[] =
        {LogLevel::TRACE, LogLevel::DEBUG, LogLevel::INFO, LogLevel::WARN, LogLevel::ERROR, LogLevel::FATAL};
    const char* expected[] = {"T", "D", "I", "W", "E", "F"};

    for (std::size_t i = 0; i < 6; ++i) {
        char buf[256] = {};
        LogRecord record {};
        record.metadata.level = levels[i];
        record.loggerName = "x";
        record.timestamp = 0;

        std::size_t len = PrintfFormatter<TimeFormat::NONE>::format(buf, sizeof(buf), record, "msg");
        std::string output(buf, len);
        std::string prefix = std::string(expected[i]) + " [x] -- msg\n";
        EXPECT_EQ(output, prefix) << "Failed for level index " << i;
    }
}

TEST(PrintfFormatter, ResourceIdFormatReturnsMessage)
{
    char buf[256] = {};
    LogRecord record {};

    std::size_t len = PrintfFormatter<TimeFormat::NONE>::format(buf, sizeof(buf), record, uint32_t {0});
    // sizeof includes the null terminator, so len includes it too
    std::string expected = "Printf formatting does not support string replacement\n";
    EXPECT_EQ(len, expected.size() + 1);
    EXPECT_EQ(std::string(buf, expected.size()), expected);
}

TEST(PrintfFormatter, ResourceIdFormatBufferTooSmall)
{
    char buf[4] = {};
    LogRecord record {};

    std::size_t len = PrintfFormatter<TimeFormat::NONE>::format(buf, sizeof(buf), record, uint32_t {0});
    EXPECT_EQ(len, 0U);
}
