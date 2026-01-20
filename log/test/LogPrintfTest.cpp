/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
#include <gtest/gtest.h>

#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/buffer/AtomicBuffer.h"
#include "se-oss/log/format/PrintfFormatter.h"
#include "se-oss/log/sink/BufferSink.h"
#include "se-oss/log/sink/FilteredSink.h"

using namespace se_oss;

namespace {

enum class LogComponents : uint8_t {
    TEST_COMP
};

constexpr const char* toString(LogComponents) {
    return "test";
}

enum class LogSinks : uint8_t {
    BUFFER_SINK
};

constexpr std::size_t LOG_BUFFER_SIZE {2048U};
constexpr std::size_t LOG_MAX_MESSAGE_LENGTH {128U};

// Helper to convert vector<uint8_t> to string
std::string bufferToString(const std::vector<uint8_t>& buffer) {
    std::string result;
    result.reserve(buffer.size());
    for (uint8_t byte : buffer) {
        if (byte != '\0') {
            result.push_back(static_cast<char>(byte));
        }
    }
    return result;
}

} // namespace

template <>
auto se_oss::logConf<>()
{
    return LogConf<PrintfFormatter<TimeFormat::HEX_8>, AtomicBuffer<LOG_BUFFER_SIZE>, LOG_MAX_MESSAGE_LENGTH>{};
}

class LogPrintfTest : public ::testing::Test {
protected:
    void SetUp() override {
        _logRegistry = std::make_unique<LogRegistry<LogComponents, LogSinks>>();
        
        // Mock time: 1234567890000 microseconds
        // 1234567890000 (0x11F71FB04D0) -> truncated to uint32_t -> 0x71FB04D0
        _logRegistry->setTimeProvider([]() {
            return 1234567890000ULL; 
        });

        _buffer.clear();
        auto bufferSink = std::make_unique<FilteredSink<BufferSink>>(_buffer);
        
        // Set up sink
        _logRegistry->attachSink(LogSinks::BUFFER_SINK, std::move(bufferSink));
        _logRegistry->getSink(LogSinks::BUFFER_SINK).setLogLevel(se_oss::LogLevel::TRACE);
    }

    void TearDown() override {
        _logRegistry.reset();
    }

    std::unique_ptr<LogRegistry<LogComponents, LogSinks>> _logRegistry;
    std::vector<uint8_t> _buffer;
};

TEST_F(LogPrintfTest, TestAllSeverities) {
    Logger& logger = _logRegistry->createOrGetLogger(LogComponents::TEST_COMP);
    logger.setLogLevel(se_oss::LogLevel::TRACE);

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
    while(std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\n') {
             line.pop_back(); 
        }
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 6);

    EXPECT_TRUE(lines[0] == "71FB0450 T [test] -- Test Trace 1");
    EXPECT_TRUE(lines[1] == "71FB0450 D [test] -- Test Debug debug");
    EXPECT_TRUE(lines[2] == "71FB0450 I [test] -- Test Info");
    EXPECT_TRUE(lines[3] == "71FB0450 W [test] -- Test Warn 100");
    EXPECT_TRUE(lines[4] == "71FB0450 E [test] -- Test Error 1.500000");
    EXPECT_TRUE(lines[5] == "71FB0450 F [test] -- Test Fatal");
}

TEST_F(LogPrintfTest, TestLogLevelFiltering) {
    Logger& logger = _logRegistry->createOrGetLogger(LogComponents::TEST_COMP);
    logger.setLogLevel(se_oss::LogLevel::INFO); // Only INFO and above

    LOG_TRACE(logger, "Hidden Trace");
    LOG_DEBUG(logger, "Hidden Debug");
    LOG_INFO(logger, "Visible Info");
    LOG_WARN(logger, "Visible Warn");

    _logRegistry->distributeMessages();
    
    std::string content = bufferToString(_buffer);
    std::stringstream ss(content);
    std::string line;
    std::vector<std::string> lines;
    while(std::getline(ss, line)) {
        if (!line.empty() && line.back() == '\n') {
             line.pop_back();
        }
        lines.push_back(line);
    }

    ASSERT_EQ(lines.size(), 2);

    EXPECT_TRUE(lines[0] == "71FB0450 I [test] -- Visible Info");
    EXPECT_TRUE(lines[1] == "71FB0450 W [test] -- Visible Warn");
}
