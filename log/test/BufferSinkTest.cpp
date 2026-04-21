/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "WriterMock.h"
#include "se-oss/log/sink/BufferSink.h"

#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class BufferSinkTest : public Test
{
};

TEST_F(BufferSinkTest, ForwardToBuffer)
{
    std::vector<uint8_t> buffer;
    BufferSink sink {buffer};

    std::string message = "Hello World\n";
    sink.write(message.data(), message.size());

    std::string output(buffer.begin(), buffer.end());
    EXPECT_EQ(output, message);
}

TEST_F(BufferSinkTest, WriteNullData_NoOp)
{
    std::vector<uint8_t> buffer;
    BufferSink sink {buffer};

    sink.write(nullptr, 10);
    EXPECT_TRUE(buffer.empty());
}

TEST_F(BufferSinkTest, WriteZeroLength_NoOp)
{
    std::vector<uint8_t> buffer;
    BufferSink sink {buffer};

    uint8_t data[] = {1, 2, 3};
    sink.write(data, 0);
    EXPECT_TRUE(buffer.empty());
}

TEST_F(BufferSinkTest, Flush_NoOp)
{
    std::vector<uint8_t> buffer;
    BufferSink sink {buffer};
    EXPECT_NO_THROW(sink.flush());
}
