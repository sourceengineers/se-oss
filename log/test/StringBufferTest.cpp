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

TEST(StringBuffer, InvalidBuffer)
{
    StringBuffer sb(nullptr, 100U);

    sb.append("hello");
    EXPECT_EQ(sb.length(), 0U);
}

TEST(StringBuffer, AppendSimpleString)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("hello");
    EXPECT_EQ(sb.length(), 5U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_STREQ(buf, "hello");
}

TEST(StringBuffer, AppendFormattedString)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("%d %s", 42, "world");
    EXPECT_EQ(sb.length(), 8U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_STREQ(buf, "42 world");
}

TEST(StringBuffer, AppendFromattedTruncation)
{
    char buf[16] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("some string: %s", "another string");
    EXPECT_EQ(sb.length(), 15U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_EQ(std::string(buf, 15), "some string: an");

    sb.append("this won't fit: %u", 42U);
    // Buffer is already full, so we expect no change in the output
    EXPECT_EQ(sb.length(), 15U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_EQ(std::string(buf, 15), "some string: an");
}

TEST(StringBuffer, AppendNullString)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append(nullptr);
    EXPECT_EQ(sb.length(), 0U);
    EXPECT_EQ(buf[sb.length()], '\0');
}

TEST(StringBuffer, AppendTruncation)
{
    // Buffer of 8 bytes: 7 usable (1 reserved for termination)
    char buf[8] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("abcdefghijklmnop");
    EXPECT_EQ(sb.length(), 7U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_EQ(std::string(buf, 7), "abcdefg");

    sb.append("0123456789");
    // Buffer is already full, so we expect no change in the output
    EXPECT_EQ(sb.length(), 7U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_EQ(std::string(buf, 7), "abcdefg");
}

TEST(StringBuffer, AppendBufferSize)
{
    // Buffer of 8 bytes: 7 usable (1 reserved for termination)
    char buf[8] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("abcdefgh");
    // The buffer reserves 1 character for the termination
    EXPECT_EQ(sb.length(), 7U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_EQ(std::string(buf, 7), "abcdefg");
}

TEST(StringBuffer, AppendMaxSize)
{
    // Buffer of 8 bytes: 7 usable (1 reserved for termination)
    char buf[8] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("abcdefg");
    EXPECT_EQ(sb.length(), 7U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_EQ(std::string(buf, 7), "abcdefg");
}

TEST(StringBuffer, AppendMultiple)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("hello");
    sb.append(" ");
    sb.append("world");
    EXPECT_EQ(sb.length(), 11U);
    EXPECT_EQ(buf[sb.length()], '\0');
    EXPECT_STREQ(buf, "hello world");
}

TEST(StringBuffer, SnprintfError)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.append("invalid format %ls", "42");
    EXPECT_EQ(sb.length(), 0U);
    EXPECT_EQ(buf[sb.length()], '\0');

    // Further append have no effect
    sb.append("hello");
    EXPECT_EQ(sb.length(), 0U);
    EXPECT_EQ(buf[sb.length()], '\0');

    sb.append("%d", 999);
    EXPECT_EQ(sb.length(), 0U);
    EXPECT_EQ(buf[sb.length()], '\0');
}

TEST(StringBuffer, AppendTimeNone)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::NONE>(12345);
    EXPECT_EQ(sb.length(), 0U);
}

TEST(StringBuffer, AppendTimeDecimal)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::DECIMAL>(12345);
    EXPECT_STREQ(buf, "12345 ");
}

TEST(StringBuffer, AppendTimeDecimal8Wrap)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    // 200000042 % 100000000 = 42
    sb.appendTime<TimeFormat::DECIMAL_8>(200000042);
    EXPECT_STREQ(buf, "00000042 ");
}

TEST(StringBuffer, AppendTimeDecimal8NoWrap)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));
    // Value below wrap threshold (100000000)
    sb.appendTime<TimeFormat::DECIMAL_8>(42);
    EXPECT_STREQ(buf, "00000042 ");
}

TEST(StringBuffer, AppendTimeDecimal10)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::DECIMAL_10>(1234567890);
    EXPECT_STREQ(buf, "1234567890 ");
}

TEST(StringBuffer, AppendTimeDecimal10Wrap)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    // 20000000042 % 10000000000 = 42
    sb.appendTime<TimeFormat::DECIMAL_10>(20000000042ULL);
    EXPECT_STREQ(buf, "0000000042 ");
}

TEST(StringBuffer, AppendTimeHex)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::HEX>(255);
    EXPECT_STREQ(buf, "FF ");
}

TEST(StringBuffer, AppendTimeHex8)
{
    char buf[64] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::HEX_8>(0xDEADBEEF);
    EXPECT_STREQ(buf, "DEADBEEF ");
}

TEST(StringBuffer, AppendTimeISO8601)
{
    char buf[128] = {};
    StringBuffer sb(buf, sizeof(buf));

    // 1737366731209138 microseconds since epoch
    // = 1737366731209 milliseconds
    // = 1737366731 seconds -> 2025-01-20T09:52:11 UTC
    // milliseconds remainder: 1737366731209 % 1000 = 209
    sb.appendTime<TimeFormat::ISO8601>(1737366731209138ULL);
    EXPECT_EQ(std::string(buf, sb.length()), "2025-01-20T09:52:11.209Z ");
}

TEST(StringBuffer, AppendTimeISO8601Error)
{
    char buf[16] = {};
    StringBuffer sb(buf, sizeof(buf));

    sb.appendTime<TimeFormat::ISO8601>(1737366731209138ULL);
    EXPECT_EQ(sb.length(), 0U);
    EXPECT_EQ(buf[sb.length()], '\0');

    // Buffer is invalid, further appends fail
    sb.appendTime<TimeFormat::ISO8601>(1737366731209138ULL);
    EXPECT_EQ(sb.length(), 0U);
    EXPECT_EQ(buf[sb.length()], '\0');

    sb.append("hello");
    EXPECT_EQ(sb.length(), 0U);
    EXPECT_EQ(buf[sb.length()], '\0');
}

