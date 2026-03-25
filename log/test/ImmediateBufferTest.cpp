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
