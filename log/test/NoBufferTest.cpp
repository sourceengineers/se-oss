/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/buffer/NoBuffer.h"

#include <cstring>

#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class NoBufferTest : public Test
{
};

TEST_F(NoBufferTest, InitialState)
{
    NoBuffer<128> buffer;
    EXPECT_EQ(buffer.capacity(), 128);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 128);
}

TEST_F(NoBufferTest, ReadWrite)
{
    std::srand(std::time({}));
    NoBuffer<128> buffer;

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
