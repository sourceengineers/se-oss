/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "WriterMock.h"
#include "se-oss/log/sink/ConsoleSink.h"

#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class ConsoleSinkTest : public Test
{
};

TEST_F(ConsoleSinkTest, ForwardToConsole)
{
    internal::CaptureStdout();
    ConsoleSink sink;

    std::string message = "Hello World\n";
    sink.write(message.data(), message.size());
    auto output = internal::GetCapturedStdout();
    EXPECT_EQ(output, message);
}

TEST_F(ConsoleSinkTest, HexOutput)
{
    internal::CaptureStdout();
    ConsoleSink sink {true};

    std::string message = "Hello World";
    std::string hexMessage = "48656C6C6F20576F726C64\n";
    sink.write(message.data(), message.size());
    auto output = internal::GetCapturedStdout();
    EXPECT_EQ(output, hexMessage);
}

TEST_F(ConsoleSinkTest, Flush)
{
    ConsoleSink sink {};
    // nothing happens when flushing in console mode
    sink.flush();
}
