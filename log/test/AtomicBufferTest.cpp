/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/sink/ILogSink.h"

#include <cstring>

#include <gtest/gtest.h>
#include <se-oss/log/buffer/AtomicBuffer.h>

#include <vector>

using namespace se_oss;

class AtomicBufferTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        // Shared setup logic if needed
    }

    void TearDown() override
    {
        // Shared teardown logic if needed
    }
};

TEST_F(AtomicBufferTest, InitialState)
{
    AtomicBuffer<1024> buffer;
    EXPECT_EQ(buffer.capacity(), 1024);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 1024);
}

TEST_F(AtomicBufferTest, SimpleWriteRead)
{
    AtomicBuffer<1024> buffer;
    const std::string data = "Hello World";

    auto writeRegion = buffer.reserveWrite(data.size());
    ASSERT_NE(writeRegion.data, nullptr);
    EXPECT_EQ(writeRegion.size, data.size());
    std::memcpy(writeRegion.data, data.data(), writeRegion.size);
    buffer.commitWrite(writeRegion.size);
    EXPECT_EQ(buffer.size(), data.size());
    EXPECT_EQ(buffer.free(), 1024 - data.size());

    std::string readData;
    readData.resize(data.size());
    auto readRegion = buffer.acquireRead();
    ASSERT_NE(readRegion.data, nullptr);
    EXPECT_EQ(readRegion.size, readData.size());
    std::memcpy(&readData[0], readRegion.data, readRegion.size);
    buffer.consumeRead(readRegion.size);
    EXPECT_EQ(readData, data);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 1024);
}

TEST_F(AtomicBufferTest, WriteFullBuffer)
{
    AtomicBuffer<10> buffer;

    std::vector<uint8_t> data(10, 0xFF);
    auto writeRegion = buffer.reserveWrite(data.size());
    ASSERT_NE(writeRegion.data, nullptr);
    std::memcpy(writeRegion.data, data.data(), writeRegion.size);
    buffer.commitWrite(writeRegion.size);
    EXPECT_EQ(buffer.size(), 10);

    auto writeMore = buffer.reserveWrite(2U);
    EXPECT_EQ(writeMore.data, nullptr);
    EXPECT_EQ(writeMore.size, 0U);
}

TEST_F(AtomicBufferTest, WrapAround)
{
    AtomicBuffer<20> buffer;

    // Write 15 bytes
    auto w1 = buffer.reserveWrite(15U);
    ASSERT_NE(w1.data, nullptr);
    std::memset(w1.data, 'A', w1.size);
    buffer.commitWrite(w1.size);
    EXPECT_EQ(buffer.size(), 15);
    EXPECT_EQ(buffer.free(), 5);

    // Read 10 bytes
    size_t bytesToRead = 10;
    auto r1 = buffer.acquireRead();
    ASSERT_NE(r1.data, nullptr);
    size_t consumed = std::min(r1.size, bytesToRead);
    bytesToRead -= consumed;
    buffer.consumeRead(consumed);
    EXPECT_EQ(buffer.size(), 5);
    EXPECT_EQ(buffer.free(), 15);
    // Reader is now at 10. Writer is at 15.

    // Now write 11 bytes.
    // Reader is at 10.
    auto w2Fail = buffer.reserveWrite(11U);
    EXPECT_EQ(w2Fail.data, nullptr);

    // Try writing 9 bytes (should succeed). Writer wraps to 0.
    auto w2 = buffer.reserveWrite(9U);
    ASSERT_NE(w2.data, nullptr);
    std::memset(w2.data, 'B', w2.size);
    buffer.commitWrite(w2.size);
    // Size: 5 (old) + 9 (new) = 14
    EXPECT_EQ(buffer.size(), 14);
    EXPECT_EQ(buffer.free(), 0);  // the last 6 bytes are not usable due to watermark

    // Read 14 bytes and reset watermark
    bytesToRead = 5;
    auto r2 = buffer.acquireRead();
    ASSERT_NE(r2.data, nullptr);
    EXPECT_EQ(bytesToRead, r2.size);
    consumed = std::min(r2.size, bytesToRead);
    bytesToRead -= consumed;
    buffer.consumeRead(consumed);
    EXPECT_EQ(buffer.size(), 9);

    bytesToRead = 9;
    auto r3 = buffer.acquireRead();
    ASSERT_NE(r3.data, nullptr);
    EXPECT_EQ(bytesToRead, r3.size);
    consumed = std::min(r3.size, bytesToRead);
    bytesToRead -= consumed;
    buffer.consumeRead(consumed);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 20);
}

TEST_F(AtomicBufferTest, ReadEmpty)
{
    AtomicBuffer<1024> buffer;
    auto region = buffer.acquireRead();
    EXPECT_EQ(region.data, nullptr);
    EXPECT_EQ(region.size, 0U);
}

TEST_F(AtomicBufferTest, CommitWrite_ZeroBytes_NoStateChange)
{
    AtomicBuffer<1024> buffer;
    auto sizeBefore = buffer.size();
    auto freeBefore = buffer.free();

    ASSERT_NE(buffer.reserveWrite(10U).data, nullptr);
    buffer.commitWrite(0U);
    EXPECT_EQ(buffer.size(), sizeBefore);
    EXPECT_EQ(buffer.free(), freeBefore);
}

TEST_F(AtomicBufferTest, Write_WriterBehindReader_InsufficientSpace)
{
    AtomicBuffer<20> buffer;

    // Fill 15 bytes
    auto region = buffer.reserveWrite(15U);
    std::memset(region.data, 'A', region.size);
    buffer.commitWrite(region.size);

    // Read 15 bytes to advance reader to 15
    auto readRegion = buffer.acquireRead();
    buffer.consumeRead(readRegion.size);

    // Write 10 bytes, writer wraps to 0
    region = buffer.reserveWrite(10U);
    std::memset(region.data, 'B', region.size);
    buffer.commitWrite(region.size);
    // Now writer=10, reader=15. Writer is behind reader.

    // Try to reserve 6 bytes: reader - writer = 5, which is <= 6 → fail
    EXPECT_EQ(buffer.reserveWrite(6U).data, nullptr);
}

TEST_F(AtomicBufferTest, Read_ReaderAtWatermark_Resets)
{
    AtomicBuffer<20> buffer;

    // Write 15 bytes (writer at 15)
    auto writeRegion = buffer.reserveWrite(15U);
    std::memset(writeRegion.data, 'A', writeRegion.size);
    buffer.commitWrite(writeRegion.size);

    // Read all 15 bytes (reader at 15)
    auto readRegion = buffer.acquireRead();
    buffer.consumeRead(readRegion.size);

    // Write 10 bytes, wraps: watermark set to 15, writer at 10
    writeRegion = buffer.reserveWrite(10U);
    std::memset(writeRegion.data, 'C', writeRegion.size);
    buffer.commitWrite(writeRegion.size);

    // Reader is at 15, watermark is at 15 → reader >= watermark triggers reset
    readRegion = buffer.acquireRead();
    ASSERT_NE(readRegion.data, nullptr);
    buffer.consumeRead(readRegion.size);
    EXPECT_EQ(buffer.size(), 0);
}

TEST_F(AtomicBufferTest, ReserveWrite_ZeroBytes_ReturnsEmptyRegion)
{
    AtomicBuffer<1024> buffer;
    auto region = buffer.reserveWrite(0U);
    EXPECT_EQ(region.data, nullptr);
    EXPECT_EQ(region.size, 0U);
}
