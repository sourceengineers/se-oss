// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "se-oss/log/sink/ILogSink.h"

#include <cstdint>

#include <gtest/gtest.h>

using namespace se_oss;

TEST(LogHeader, Serialize_NullBuffer_ReturnsNull)
{
    LogHeader header {};
    EXPECT_EQ(serialize(header, nullptr, LogHeader::PACKED_SIZE), nullptr);
}

TEST(LogHeader, Serialize_BufferTooSmall_ReturnsNull)
{
    LogHeader header {};
    uint8_t buffer[4] {};
    EXPECT_EQ(serialize(header, buffer, 4), nullptr);
}

TEST(LogHeader, Deserialize_NullBuffer_ReturnsNull)
{
    LogHeader header {};
    EXPECT_EQ(deserialize(header, nullptr, LogHeader::PACKED_SIZE), nullptr);
}

TEST(LogHeader, Deserialize_BufferTooSmall_ReturnsNull)
{
    LogHeader header {};
    uint8_t buffer[4] {};
    EXPECT_EQ(deserialize(header, buffer, 4), nullptr);
}

TEST(LogHeader, SerializeDeserialize_RoundTrip)
{
    LogHeader original {};
    original.metadata.level = LogLevel::WARN;
    original.metadata.contextTag = 42;
    original.metadata.loggerTag = 7;
    original.messageLength = 1234;

    uint8_t buffer[LogHeader::PACKED_SIZE] {};
    void* end = serialize(original, buffer, sizeof(buffer));
    ASSERT_NE(end, nullptr);
    EXPECT_EQ(end, buffer + LogHeader::PACKED_SIZE);

    LogHeader deserialized {};
    const void* readEnd = deserialize(deserialized, buffer, sizeof(buffer));
    ASSERT_NE(readEnd, nullptr);
    EXPECT_EQ(readEnd, buffer + LogHeader::PACKED_SIZE);

    EXPECT_EQ(deserialized.metadata.level, LogLevel::WARN);
    EXPECT_EQ(deserialized.metadata.contextTag, 42);
    EXPECT_EQ(deserialized.metadata.loggerTag, 7);
    EXPECT_EQ(deserialized.messageLength, 1234);
}

TEST(LogHeader, WriteValueDoesNotFit)
{
    std::array<uint8_t, 3> buffer {};
    uint32_t value {42U};
    EXPECT_EQ(writeValue(value, buffer.data(), buffer.end()), nullptr);
    EXPECT_EQ(writeValue(value, nullptr, buffer.end()), nullptr);
}

TEST(LogHeader, ReaderValueDoesNotFit)
{
    std::array<uint8_t, 3> buffer {};
    uint32_t value {42U};
    EXPECT_EQ(readValue(value, buffer.data(), buffer.end()), nullptr);
    EXPECT_EQ(readValue(value, nullptr, buffer.end()), nullptr);
}
