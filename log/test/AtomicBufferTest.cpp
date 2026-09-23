/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/sink/ILogSink.h"

#include <cstring>
#include <thread>

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

TEST_F(AtomicBufferTest, RejectsReservationLargerThanCapacity)
{
    AtomicBuffer<10> buffer;

    auto writeRegion = buffer.reserveWrite(11U);

    EXPECT_EQ(writeRegion.data, nullptr);
    EXPECT_EQ(writeRegion.size, 0U);
    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_EQ(buffer.free(), buffer.capacity());
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

TEST_F(AtomicBufferTest, RejectsOverlappingOperations)
{
    AtomicBuffer<20> buffer;

    auto writeRegion = buffer.reserveWrite(10U);
    ASSERT_NE(writeRegion.data, nullptr);
    EXPECT_EQ(buffer.reserveWrite(1U).data, nullptr);
    buffer.commitWrite(writeRegion.size);

    auto readRegion = buffer.acquireRead();
    ASSERT_NE(readRegion.data, nullptr);
    EXPECT_EQ(buffer.acquireRead().data, nullptr);
    buffer.consumeRead(readRegion.size);
}

TEST_F(AtomicBufferTest, CompletionWithoutOutstandingOperationHasNoEffect)
{
    AtomicBuffer<20> buffer;
    auto writeRegion = buffer.reserveWrite(10U);
    ASSERT_NE(writeRegion.data, nullptr);
    buffer.commitWrite(writeRegion.size);

    auto readRegion = buffer.acquireRead();
    ASSERT_NE(readRegion.data, nullptr);
    buffer.consumeRead(4U);
    auto sizeBefore = buffer.size();
    auto freeBefore = buffer.free();

    buffer.commitWrite(5U);
    buffer.consumeRead(5U);

    EXPECT_EQ(buffer.size(), sizeBefore);
    EXPECT_EQ(buffer.free(), freeBefore);
}

TEST_F(AtomicBufferTest, WriterReachingPhysicalEndWithReaderAtZero_KeepsGuardByte)
{
    // Physical storage is capacity + 1. With the reader at 0, the last physical slot is the guard byte, so a
    // reservation that would land the writer exactly on the physical end must be rejected even though the
    // contiguous tail is large enough.
    AtomicBuffer<4> buffer;

    auto writeRegion = buffer.reserveWrite(4U);
    ASSERT_NE(writeRegion.data, nullptr);
    std::memset(writeRegion.data, 'A', writeRegion.size);
    buffer.commitWrite(writeRegion.size);
    EXPECT_EQ(buffer.size(), 4U);
    EXPECT_EQ(buffer.free(), 0U);

    EXPECT_EQ(buffer.reserveWrite(1U).data, nullptr);
    EXPECT_EQ(buffer.size(), 4U);
    EXPECT_EQ(buffer.free(), 0U);

    auto readRegion = buffer.acquireRead();
    ASSERT_EQ(readRegion.size, 4U);
    buffer.consumeRead(readRegion.size);

    EXPECT_EQ(buffer.acquireRead().data, nullptr);
    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_EQ(buffer.free(), 4U);
}

TEST_F(AtomicBufferTest, WriterReachingPhysicalEndWithReaderAhead_DrainsToEmpty)
{
    // reader = 2, writer lands exactly on the physical end (5). After draining, reader == writer == watermark. This
    // must be read as empty, not as a full buffer starting at 0.
    AtomicBuffer<4> buffer;

    auto writeRegion = buffer.reserveWrite(2U);
    ASSERT_NE(writeRegion.data, nullptr);
    std::memset(writeRegion.data, 'X', writeRegion.size);
    buffer.commitWrite(writeRegion.size);
    auto readRegion = buffer.acquireRead();
    ASSERT_EQ(readRegion.size, 2U);
    buffer.consumeRead(readRegion.size);

    writeRegion = buffer.reserveWrite(3U);
    ASSERT_NE(writeRegion.data, nullptr);
    std::memset(writeRegion.data, 'Y', writeRegion.size);
    buffer.commitWrite(writeRegion.size);
    EXPECT_EQ(buffer.size(), 3U);

    readRegion = buffer.acquireRead();
    ASSERT_EQ(readRegion.size, 3U);
    EXPECT_EQ(std::memcmp(readRegion.data, "YYY", 3U), 0);
    buffer.consumeRead(readRegion.size);

    EXPECT_EQ(buffer.size(), 0U);
    EXPECT_EQ(buffer.free(), 4U);
    EXPECT_EQ(buffer.acquireRead().data, nullptr);

    // The producer must still be able to wrap and the consumer must follow it to the beginning.
    writeRegion = buffer.reserveWrite(2U);
    ASSERT_NE(writeRegion.data, nullptr);
    std::memset(writeRegion.data, 'Z', writeRegion.size);
    buffer.commitWrite(writeRegion.size);

    readRegion = buffer.acquireRead();
    ASSERT_EQ(readRegion.size, 2U);
    EXPECT_EQ(std::memcmp(readRegion.data, "ZZ", 2U), 0);
    buffer.consumeRead(readRegion.size);
    EXPECT_EQ(buffer.size(), 0U);
}

TEST_F(AtomicBufferTest, ConcurrentVariableChunkSizesPreserveByteSequence)
{
    // Cycling chunk sizes make the writer visit every tail offset, including landing exactly on the physical end.
    // Fixed chunk sizes (see the test below) never reach that state.
    constexpr std::size_t numberOfBytes {2000000U};
    AtomicBuffer<64> buffer;
    std::atomic<bool> producerFinished {false};

    std::thread producer([&]() {
        std::size_t produced {0U};
        std::size_t iteration {0U};
        while (produced < numberOfBytes) {
            auto requested = std::min<std::size_t>(1U + (iteration++ % 13U), numberOfBytes - produced);
            auto region = buffer.reserveWrite(requested);
            if (region.data == nullptr) {
                std::this_thread::yield();
                continue;
            }
            auto* bytes = static_cast<uint8_t*>(region.data);
            for (std::size_t i = 0U; i < region.size; ++i) {
                bytes[i] = static_cast<uint8_t>((produced + i) % 251U);
            }
            buffer.commitWrite(region.size);
            produced += region.size;
        }
        producerFinished.store(true);
    });

    std::size_t consumed {0U};
    std::size_t overReads {0U};
    std::size_t mismatches {0U};
    while (!producerFinished.load() || consumed < numberOfBytes) {
        auto region = buffer.acquireRead();
        if (region.data == nullptr) {
            std::this_thread::yield();
            continue;
        }
        if (consumed + region.size > numberOfBytes) {
            ++overReads;
        }
        auto* bytes = static_cast<const uint8_t*>(region.data);
        for (std::size_t i = 0U; i < region.size && consumed + i < numberOfBytes; ++i) {
            if (bytes[i] != static_cast<uint8_t>((consumed + i) % 251U)) {
                ++mismatches;
            }
        }
        buffer.consumeRead(region.size);
        consumed += region.size;
    }
    producer.join();

    EXPECT_EQ(mismatches, 0U);
    EXPECT_EQ(overReads, 0U);
    EXPECT_EQ(consumed, numberOfBytes);
    EXPECT_EQ(buffer.size(), 0U);
}

TEST_F(AtomicBufferTest, ConcurrentProducerConsumerPreservesByteSequence)
{
    constexpr std::size_t numberOfBytes {500000U};
    AtomicBuffer<127> buffer;
    std::atomic<bool> producerFinished {false};
    std::atomic<bool> mismatch {false};
    std::atomic<bool> invalidReportedSize {false};

    std::thread producer([&]() {
        std::size_t produced {0U};
        while (produced < numberOfBytes) {
            auto requested = std::min<std::size_t>(17U, numberOfBytes - produced);
            auto region = buffer.reserveWrite(requested);
            if (region.data == nullptr) {
                std::this_thread::yield();
                continue;
            }
            auto* bytes = static_cast<uint8_t*>(region.data);
            for (std::size_t i = 0U; i < region.size; ++i) {
                bytes[i] = static_cast<uint8_t>((produced + i) % 251U);
            }
            buffer.commitWrite(region.size);
            produced += region.size;
        }
        producerFinished.store(true);
    });

    std::size_t consumed {0U};
    while (!producerFinished.load() || consumed < numberOfBytes) {
        if (buffer.size() > buffer.capacity() || buffer.free() > buffer.capacity()) {
            invalidReportedSize.store(true);
        }
        auto region = buffer.acquireRead();
        if (region.data == nullptr) {
            std::this_thread::yield();
            continue;
        }
        auto bytesToConsume = std::min<std::size_t>(11U, region.size);
        auto* bytes = static_cast<const uint8_t*>(region.data);
        for (std::size_t i = 0U; i < bytesToConsume; ++i) {
            if (bytes[i] != static_cast<uint8_t>((consumed + i) % 251U)) {
                mismatch.store(true);
            }
        }
        buffer.consumeRead(bytesToConsume);
        consumed += bytesToConsume;
    }
    producer.join();

    EXPECT_FALSE(mismatch.load());
    EXPECT_FALSE(invalidReportedSize.load());
    EXPECT_EQ(consumed, numberOfBytes);
    EXPECT_EQ(buffer.size(), 0U);
}
