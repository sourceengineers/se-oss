/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/buffer/AtomicBuffer.h"
#include "se-oss/log/format/PrintfFormatter.h"
#include "se-oss/log/sink/BufferSink.h"
#include "se-oss/log/sink/FilteredSink.h"

#include <algorithm>

#include <gtest/gtest.h>

#include <cinttypes>
#include <cstddef>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

// the format check cases below test the check only when it is on (see log/test/CMakeLists.txt)
#ifndef SE_OSS_LOG_CHECK_PRINTF_FORMAT
#error "se_log_tests must be compiled with SE_OSS_LOG_CHECK_PRINTF_FORMAT"
#endif

using namespace se_oss;

namespace {

enum class LogComponents : uint8_t
{
    TEST_COMP
};

constexpr const char* toString(LogComponents) { return "test"; }

enum class LogSinks : uint8_t
{
    BUFFER_SINK
};

// Helper to convert vector<uint8_t> to string
std::string bufferToString(const std::vector<uint8_t>& buffer)
{
    std::string result;
    result.reserve(buffer.size());
    for (uint8_t byte : buffer) {
        if (byte != '\0') {
            result.push_back(static_cast<char>(byte));
        }
    }
    return result;
}

std::vector<std::string> bufferToLines(const std::vector<uint8_t>& buffer)
{
    std::stringstream ss(bufferToString(buffer));
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(ss, line)) {
        lines.push_back(line);
    }
    return lines;
}

}  // namespace

class LogPrintfTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        _logRegistry = std::make_unique<LogRegistry<LogComponents, LogSinks>>();

        // Mock time: 1234567890000 microseconds since epoch
        // ISO8601: 1970-01-15T06:56:07.890Z
        _logRegistry->setTimeProvider([]() -> uint64_t { return 1234567890000ULL; });

        _buffer.clear();
        auto bufferSink = std::make_unique<FilteredSink<BufferSink>>(_buffer);

        // Set up sink
        _logRegistry->attachSink(LogSinks::BUFFER_SINK, std::move(bufferSink));
        _logRegistry->getSink(LogSinks::BUFFER_SINK).setLogFilterLevel(se_oss::LogLevel::TRACE);
    }

    void TearDown() override { _logRegistry.reset(); }

    std::unique_ptr<LogRegistry<LogComponents, LogSinks>> _logRegistry;
    std::vector<uint8_t> _buffer;
};

TEST_F(LogPrintfTest, TestAllSeverities)
{
    Logger logger = _logRegistry->createLogger(LogComponents::TEST_COMP);
    logger.setLogFilterLevel(se_oss::LogLevel::TRACE);

    LOG_TRACE(logger, "Test Trace %d", 1);
    LOG_DEBUG(logger, "Test Debug %s", "debug");
    LOG_INFO(logger, "Test Info");
    LOG_WARN(logger, "Test Warn %u", 100U);
    LOG_ERROR(logger, "Test Error %f", 1.5f);
    LOG_FATAL(logger, "Test Fatal");

    _logRegistry->distributeMessages();

    std::string content = bufferToString(_buffer);
    std::stringstream ss(content);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\n') {
            line.pop_back();
        }
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 6);

    EXPECT_TRUE(lines[0] == "1970-01-15T06:56:07.890Z T [test] -- Test Trace 1");
    EXPECT_TRUE(lines[1] == "1970-01-15T06:56:07.890Z D [test] -- Test Debug debug");
    EXPECT_TRUE(lines[2] == "1970-01-15T06:56:07.890Z I [test] -- Test Info");
    EXPECT_TRUE(lines[3] == "1970-01-15T06:56:07.890Z W [test] -- Test Warn 100");
    EXPECT_TRUE(lines[4] == "1970-01-15T06:56:07.890Z E [test] -- Test Error 1.500000");
    EXPECT_TRUE(lines[5] == "1970-01-15T06:56:07.890Z F [test] -- Test Fatal");
}

TEST_F(LogPrintfTest, TestLogLevelFiltering)
{
    Logger logger = _logRegistry->createLogger(LogComponents::TEST_COMP);
    logger.setLogFilterLevel(se_oss::LogLevel::INFO);  // Only INFO and above

    LOG_TRACE(logger, "Hidden Trace");
    LOG_DEBUG(logger, "Hidden Debug");
    LOG_INFO(logger, "Visible Info");
    LOG_WARN(logger, "Visible Warn");

    _logRegistry->distributeMessages();

    std::string content = bufferToString(_buffer);
    std::stringstream ss(content);
    std::string line;
    std::vector<std::string> lines;
    while (std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\n') {
            line.pop_back();
        }
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 2);

    EXPECT_TRUE(lines[0] == "1970-01-15T06:56:07.890Z I [test] -- Visible Info");
    EXPECT_TRUE(lines[1] == "1970-01-15T06:56:07.890Z W [test] -- Visible Warn");
}

TEST_F(LogPrintfTest, TestLogMessageWithRuntimeLevel)
{
    Logger logger = _logRegistry->createLogger(LogComponents::TEST_COMP);
    logger.setLogFilterLevel(se_oss::LogLevel::TRACE);

    const std::vector<LogLevel> levels {LogLevel::TRACE, LogLevel::DEBUG, LogLevel::INFO, LogLevel::WARN,
                                        LogLevel::ERROR, LogLevel::FATAL, LogLevel::OFF};
    for (const LogLevel level : levels) {
        LOG_MESSAGE(logger, level, "Level %s", toString(level));
    }
    LOG_MESSAGE(logger, LogLevel::INFO, "Without arguments");

    _logRegistry->distributeMessages();

    const std::vector<std::string> lines = bufferToLines(_buffer);
    ASSERT_EQ(lines.size(), 7);
    EXPECT_EQ(lines[0], "1970-01-15T06:56:07.890Z T [test] -- Level trace");
    EXPECT_EQ(lines[1], "1970-01-15T06:56:07.890Z D [test] -- Level debug");
    EXPECT_EQ(lines[2], "1970-01-15T06:56:07.890Z I [test] -- Level info");
    EXPECT_EQ(lines[3], "1970-01-15T06:56:07.890Z W [test] -- Level warn");
    EXPECT_EQ(lines[4], "1970-01-15T06:56:07.890Z E [test] -- Level error");
    EXPECT_EQ(lines[5], "1970-01-15T06:56:07.890Z F [test] -- Level fatal");
    EXPECT_EQ(lines[6], "1970-01-15T06:56:07.890Z I [test] -- Without arguments");
}

TEST_F(LogPrintfTest, TestLogMessageEvaluatesTheLevelOnce)
{
    Logger logger = _logRegistry->createLogger(LogComponents::TEST_COMP);
    uint32_t evaluations {0U};
    const auto level = [&evaluations]() {
        ++evaluations;
        return LogLevel::WARN;
    };

    LOG_MESSAGE(logger, level(), "Evaluated once");

    _logRegistry->distributeMessages();

    EXPECT_EQ(evaluations, 1U);
    const std::vector<std::string> lines = bufferToLines(_buffer);
    ASSERT_EQ(lines.size(), 1);
    EXPECT_EQ(lines[0], "1970-01-15T06:56:07.890Z W [test] -- Evaluated once");
}

TEST_F(LogPrintfTest, TestPortableSpecifiersPassTheFormatCheck)
{
    Logger logger = _logRegistry->createLogger(LogComponents::TEST_COMP);
    logger.setLogFilterLevel(se_oss::LogLevel::TRACE);
    const uint32_t largeValue {4000000000U};
    const uint8_t byteValue {0xFFU};
    const std::size_t count {7U};

    LOG_INFO(logger, "u32 %" PRIu32 " x8 %02" PRIX8 " size %zu name %s", largeValue, byteValue, count, "stack");

    _logRegistry->distributeMessages();

    const std::vector<std::string> lines = bufferToLines(_buffer);
    ASSERT_EQ(lines.size(), 1);
    EXPECT_EQ(lines[0], "1970-01-15T06:56:07.890Z I [test] -- u32 4000000000 x8 FF size 7 name stack");
}

TEST_F(LogPrintfTest, TestFormatCheckDoesNotEvaluateTheArguments)
{
    Logger logger = _logRegistry->createLogger(LogComponents::TEST_COMP);
    logger.setLogFilterLevel(se_oss::LogLevel::TRACE);
    uint32_t evaluations {0U};
    const auto value = [&evaluations]() {
        ++evaluations;
        return evaluations;
    };

    LOG_INFO(logger, "Value %" PRIu32, value());

    _logRegistry->distributeMessages();

    EXPECT_EQ(evaluations, 1U);
    const std::vector<std::string> lines = bufferToLines(_buffer);
    ASSERT_EQ(lines.size(), 1);
    EXPECT_EQ(lines[0], "1970-01-15T06:56:07.890Z I [test] -- Value 1");
}
