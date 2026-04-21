/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/buffer/ImmediateBuffer.h"

#include <cstring>

#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class ImmediateBufferTest : public Test
{
};

TEST_F(ImmediateBufferTest, InitialState)
{
    ImmediateBuffer<128> buffer;
    EXPECT_EQ(buffer.capacity(), 128 + LogHeader::PACKED_SIZE);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 128 + LogHeader::PACKED_SIZE);
}

TEST_F(ImmediateBufferTest, ReadWrite)
{
    std::srand(std::time({}));
    ImmediateBuffer<128> buffer;

    for (size_t i = 0U; i < 10U; ++i) {
        std::array<uint8_t, 42U> data {};
        std::fill(data.begin(), data.end(), std::rand());
        bool writeSuccessful = buffer.write(data.size(), [&](void* ptr, size_t size) {
            std::memcpy(ptr, data.data(), data.size());
            return size;
        });
        EXPECT_TRUE(writeSuccessful);

        std::array<uint8_t, 42U> readData;
        bool readSuccessful = buffer.read([&](const void* ptr, size_t size) {
            std::memcpy(readData.data(), ptr, readData.size());
            return size;
        });
        EXPECT_TRUE(readSuccessful);
        EXPECT_EQ(readData, data);
    }
}

TEST_F(ImmediateBufferTest, Write_ReserveTooLarge_ReturnsFalse)
{
    ImmediateBuffer<16> buffer;
    std::size_t tooLarge = buffer.capacity() + 1;
    bool result = buffer.write(tooLarge, [](void*, size_t sz) { return sz; });
    EXPECT_FALSE(result);
}

TEST_F(ImmediateBufferTest, Write_ProducerReturnsZero_ReturnsFalse)
{
    ImmediateBuffer<128> buffer;
    bool result = buffer.write(10, [](void*, size_t) -> size_t { return 0; });
    EXPECT_FALSE(result);
}

TEST_F(ImmediateBufferTest, Read_ConsumerReturnsZero_ReturnsFalse)
{
    ImmediateBuffer<128> buffer;
    // Write some data first
    buffer.write(10, [](void* ptr, size_t sz) {
        std::memset(ptr, 0xAB, 10);
        return sz;
    });
    // Consumer returns 0
    bool result = buffer.read([](const void*, size_t) -> size_t { return 0; });
    EXPECT_FALSE(result);
}
