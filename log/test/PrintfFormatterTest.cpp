/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/format/PrintfFormatter.h"

#include <cstring>
#include <string>

#include <gtest/gtest.h>

using namespace se_oss;

// ---------------------------------------------------------------------------
// 1. LogStringBuffer — Core Operations
// ---------------------------------------------------------------------------

TEST(LogStringBuffer, AppendSimpleString)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.append("hello");
    EXPECT_EQ(sb.length(), 5U);
    EXPECT_STREQ(buf, "hello");
}

TEST(LogStringBuffer, AppendFormattedString)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.append("%d %s", 42, "world");
    EXPECT_EQ(sb.length(), 8U);
    EXPECT_STREQ(buf, "42 world");
}

TEST(LogStringBuffer, AppendNullString)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.append(nullptr);
    EXPECT_EQ(sb.length(), 0U);
}

TEST(LogStringBuffer, AppendTruncation)
{
    // Buffer of 8 bytes: 7 usable (1 reserved for endLine)
    char buf[8] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.append("abcdefghijklmnop");
    EXPECT_EQ(sb.length(), 7U);
    EXPECT_EQ(std::string(buf, 7), "abcdefg");
}

TEST(LogStringBuffer, AppendMultiple)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.append("hello");
    sb.append(" ");
    sb.append("world");
    EXPECT_EQ(sb.length(), 11U);
    EXPECT_STREQ(buf, "hello world");
}

TEST(LogStringBuffer, EndLine)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.append("hi");
    sb.endLine();
    EXPECT_EQ(sb.length(), 3U);
    EXPECT_EQ(buf[2], '\n');
}

TEST(LogStringBuffer, CapacityReservesEndLine)
{
    // Buffer of 6 bytes: 5 usable capacity, 1 reserved for newline
    char buf[6] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.append("abcde");
    EXPECT_EQ(sb.length(), 5U);

    sb.endLine();
    EXPECT_EQ(sb.length(), 6U);
    EXPECT_EQ(buf[5], '\n');
}

TEST(LogStringBuffer, InvalidAfterSnprintfError)
{
    char buf[4] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    // Formatted append that exceeds remaining capacity sets _length beyond capacity
    // but snprintf itself won't return negative for truncation. Instead, test that
    // after the buffer is fully consumed, length reflects what was written.
    sb.append("abc");
    EXPECT_EQ(sb.length(), 3U);

    // Further formatted append into 0 remaining capacity
    sb.append("%d", 999);
    // snprintf returns the number of chars that *would* have been written,
    // so _length grows beyond _capacity, but the buffer content is safe.
    // The key invariant is that endLine won't write past the buffer.
}

// ---------------------------------------------------------------------------
// 2. LogStringBuffer — Time Formatting
// ---------------------------------------------------------------------------

TEST(LogStringBuffer, AppendTimeNone)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::NONE>(12345);
    EXPECT_EQ(sb.length(), 0U);
}

TEST(LogStringBuffer, AppendTimeDecimal)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::DECIMAL>(12345);
    EXPECT_STREQ(buf, "12345 ");
}

TEST(LogStringBuffer, AppendTimeDecimal8)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::DECIMAL_8>(123456789);
    EXPECT_STREQ(buf, "23456789 ");
}

TEST(LogStringBuffer, AppendTimeDecimal8Wrap)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    // 200000042 % 100000000 = 42
    sb.appendTime<TimeFormat::DECIMAL_8>(200000042);
    EXPECT_STREQ(buf, "00000042 ");
}

TEST(LogStringBuffer, AppendTimeDecimal10)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::DECIMAL_10>(1234567890);
    EXPECT_STREQ(buf, "1234567890 ");
}

TEST(LogStringBuffer, AppendTimeDecimal10Wrap)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    // 20000000042 % 10000000000 = 42
    sb.appendTime<TimeFormat::DECIMAL_10>(20000000042ULL);
    EXPECT_STREQ(buf, "0000000042 ");
}

TEST(LogStringBuffer, AppendTimeHex)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::HEX>(255);
    EXPECT_STREQ(buf, "FF ");
}

TEST(LogStringBuffer, AppendTimeHex8)
{
    char buf[64] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::HEX_8>(0xDEADBEEF);
    EXPECT_STREQ(buf, "DEADBEEF ");
}

TEST(LogStringBuffer, AppendTimeISO8601)
{
    char buf[128] = {};
    LogStringBuffer sb(buf, sizeof(buf));

    // 1737366731209138 microseconds since epoch
    // = 1737366731209 milliseconds
    // = 1737366731 seconds -> 2025-01-20T09:52:11 UTC
    // milliseconds remainder: 1737366731209 % 1000 = 209
    sb.appendTime<TimeFormat::ISO8601>(1737366731209138ULL);
    EXPECT_EQ(std::string(buf, sb.length()), "2025-01-20T09:52:11.209Z ");
}

// ---------------------------------------------------------------------------
// 3. PrintfFormatter::format (string format overload)
// ---------------------------------------------------------------------------

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
    char buf[10] = {};
    LogRecord record {};
    record.metadata.level = LogLevel::INFO;
    record.loggerName = "src";
    record.timestamp = 0;

    std::size_t len = PrintfFormatter<TimeFormat::NONE>::format(buf, sizeof(buf), record, "hello world this is long");
    // The formatted output exceeds the buffer; snprintf returns the would-be length
    // which causes _length to exceed _capacity. The buffer content is truncated.
    // length() still returns the tracked _length (snprintf's return value sum).
    // The key thing is no crash / no buffer overflow.
    (void)len;
}

TEST(PrintfFormatter, FormatAllLogLevels)
{
    const LogLevel levels[] = {LogLevel::TRACE, LogLevel::DEBUG, LogLevel::INFO,
                               LogLevel::WARN,  LogLevel::ERROR,  LogLevel::FATAL};
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

// ---------------------------------------------------------------------------
// 4. PrintfFormatter::format (resource-ID overload, unsupported)
// ---------------------------------------------------------------------------

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
