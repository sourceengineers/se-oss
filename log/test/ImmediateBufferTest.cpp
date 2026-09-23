/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
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
        auto writeRegion = buffer.reserveWrite(data.size());
        ASSERT_NE(writeRegion.data, nullptr);
        EXPECT_EQ(writeRegion.size, data.size());
        std::memcpy(writeRegion.data, data.data(), writeRegion.size);
        buffer.commitWrite(writeRegion.size);

        std::array<uint8_t, 42U> readData;
        auto readRegion = buffer.acquireRead();
        ASSERT_NE(readRegion.data, nullptr);
        EXPECT_EQ(readRegion.size, readData.size());
        std::memcpy(readData.data(), readRegion.data, readData.size());
        buffer.consumeRead(readRegion.size);
        EXPECT_EQ(readData, data);
    }
}

TEST_F(ImmediateBufferTest, ReserveWrite_TooLarge_ReturnsEmptyRegion)
{
    ImmediateBuffer<16> buffer;
    std::size_t tooLarge = buffer.capacity() + 1;
    auto region = buffer.reserveWrite(tooLarge);
    EXPECT_EQ(region.data, nullptr);
    EXPECT_EQ(region.size, 0U);
}

TEST_F(ImmediateBufferTest, CommitWrite_ZeroBytes_DoesNotAddData)
{
    ImmediateBuffer<128> buffer;
    ASSERT_NE(buffer.reserveWrite(10U).data, nullptr);
    buffer.commitWrite(0U);
    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_EQ(buffer.acquireRead().data, nullptr);
}

TEST_F(ImmediateBufferTest, ConsumeRead_PartialReadAdvancesRegion)
{
    ImmediateBuffer<128> buffer;
    auto writeRegion = buffer.reserveWrite(10U);
    ASSERT_NE(writeRegion.data, nullptr);
    std::memset(writeRegion.data, 0xAB, writeRegion.size);
    buffer.commitWrite(writeRegion.size);

    auto firstRead = buffer.acquireRead();
    buffer.consumeRead(4U);
    auto secondRead = buffer.acquireRead();

    EXPECT_EQ(secondRead.data, static_cast<const uint8_t*>(firstRead.data) + 4U);
    EXPECT_EQ(secondRead.size, 6U);
}
