/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "WriterMock.h"
#include "se-oss/log/sink/ConsoleWriter.h"

#include <gtest/gtest.h>

using namespace se_oss;
using namespace testing;

class ConsoleWriterTest : public Test
{
};

TEST_F(ConsoleWriterTest, ForwardToConsole)
{
    internal::CaptureStdout();
    ConsoleWriter sink;

    std::string message = "Hello World\n";
    sink.write(message.data(), message.size());
    auto output = internal::GetCapturedStdout();
    EXPECT_EQ(output, message);
}

TEST_F(ConsoleWriterTest, HexOutput)
{
    internal::CaptureStdout();
    ConsoleWriter sink {true};

    std::string message = "Hello World";
    std::string hexMessage = "48656C6C6F20576F726C64\n";
    sink.write(message.data(), message.size());
    auto output = internal::GetCapturedStdout();
    EXPECT_EQ(output, hexMessage);
}

TEST_F(ConsoleWriterTest, Flush)
{
    ConsoleWriter sink {};
    // nothing happens when flushing in console mode
    sink.flush();
}

TEST_F(ConsoleWriterTest, InvalidInput)
{
    internal::CaptureStdout();
    ConsoleWriter sink;

    sink.write(nullptr, 0U);
    auto output = internal::GetCapturedStdout();
    EXPECT_EQ(output, "");
}
