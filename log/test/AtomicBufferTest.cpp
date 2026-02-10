/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
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

    // Write
    bool writeResult = buffer.write(data.size(), [&](void* ptr, size_t size) {
        std::memcpy(ptr, data.data(), size);
        return size;
    });

    EXPECT_TRUE(writeResult);
    EXPECT_EQ(buffer.size(), data.size());
    EXPECT_EQ(buffer.free(), 1024 - data.size());

    // Read
    std::string readData;
    readData.resize(data.size());
    bool readResult = buffer.read([&](const void* ptr, size_t size) {
        std::memcpy(readData.data(), ptr, size);
        return size;
    });

    EXPECT_TRUE(readResult);
    EXPECT_EQ(readData, data);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 1024);
}

TEST_F(AtomicBufferTest, WriteFullBuffer)
{
    AtomicBuffer<10> buffer;

    std::vector<uint8_t> data(10, 0xFF);
    bool writeResult = buffer.write(data.size(), [&](void* ptr, size_t size) {
        std::memcpy(ptr, data.data(), size);
        return size;
    });

    EXPECT_TRUE(writeResult);
    EXPECT_EQ(buffer.size(), 10);

    bool writeMore = buffer.write(2, [&](void*, size_t sz) {
        EXPECT_TRUE(false);  // this never gets called
        return sz;
    });
    EXPECT_FALSE(writeMore);
}

TEST_F(AtomicBufferTest, WrapAround)
{
    AtomicBuffer<20> buffer;

    // Write 15 bytes
    bool w1 = buffer.write(15, [&](void* dest, size_t sz) {
        std::memset(dest, 'A', sz);
        return sz;
    });
    EXPECT_TRUE(w1);
    EXPECT_EQ(buffer.size(), 15);
    EXPECT_EQ(buffer.free(), 5);

    // Read 10 bytes
    size_t bytesToRead = 10;
    bool r1 = buffer.read([&](const void* src, size_t sz) {
        size_t consumed = std::min(sz, bytesToRead);
        bytesToRead -= consumed;
        return consumed;
    });
    EXPECT_TRUE(r1);
    EXPECT_EQ(buffer.size(), 5);
    EXPECT_EQ(buffer.free(), 15);
    // Reader is now at 10. Writer is at 15.

    // Now write 11 bytes.
    // Reader is at 10.
    bool w2_fail = buffer.write(11, [&](void* dest, size_t sz) {
        EXPECT_TRUE(false);  // this never gets called
        return sz;
    });
    EXPECT_FALSE(w2_fail);

    // Try writing 9 bytes (should succeed). Writer wraps to 0.
    bool w2 = buffer.write(9, [&](void* dest, size_t sz) {
        std::memset(dest, 'B', sz);
        return sz;
    });

    EXPECT_TRUE(w2);
    // Size: 5 (old) + 9 (new) = 14
    EXPECT_EQ(buffer.size(), 14);
    EXPECT_EQ(buffer.free(), 0);  // the last 6 bytes are not usable due to watermark

    // Read 14 bytes and reset watermark
    bytesToRead = 5;
    bool r2 = buffer.read([&](const void* src, size_t sz) {
        EXPECT_EQ(bytesToRead, sz);
        size_t consumed = std::min(sz, bytesToRead);
        bytesToRead -= consumed;
        return consumed;
    });
    EXPECT_TRUE(r2);
    EXPECT_EQ(buffer.size(), 9);

    bytesToRead = 9;
    bool r3 = buffer.read([&](const void* src, size_t sz) {
        EXPECT_EQ(bytesToRead, sz);
        size_t consumed = std::min(sz, bytesToRead);
        bytesToRead -= consumed;
        return consumed;
    });
    EXPECT_TRUE(r3);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 20);
}

TEST_F(AtomicBufferTest, ReadEmpty)
{
    AtomicBuffer<1024> buffer;
    bool readResult = buffer.read([](const void*, size_t sz) {
        EXPECT_TRUE(false);  // this never gets called
        return sz;
    });
    EXPECT_FALSE(readResult);
}
