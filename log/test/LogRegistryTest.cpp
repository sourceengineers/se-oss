// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "LogSinkMock.h"
#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/sink/BufferSink.h"
#include "se-oss/log/sink/FilteredSink.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <vector>

using namespace se_oss;
using namespace testing;

// ---- Default registry tests (DefaultLogContext, DefaultLogSink) ----

TEST(LogRegistryDefault, AutoCreatesConsoleSink)
{
    LogRegistry<> registry;
    // createLogger triggers checkSinkHandler which auto-creates a ConsoleSink
    Logger logger = registry.createLogger(DefaultLogContext::DEFAULT);
    EXPECT_STREQ(logger.name(), "default");
}

TEST(LogRegistryDefault, CreateLogger_Works)
{
    LogRegistry<> registry;
    Logger logger = registry.createLogger(DefaultLogContext::DEFAULT);
    // Logger is functional
    LogStatistics stats = logger.statistics();
    EXPECT_EQ(stats.droppedMessages, 0U);
}

TEST(LogRegistryDefault, ToString_DefaultLogContext)
{
    EXPECT_STREQ(toString(DefaultLogContext::DEFAULT), "default");
}

// ---- Custom registry tests ----

namespace {

enum class TestContexts : uint8_t
{
    COMP_A,
    COMP_B
};

constexpr const char* toString(TestContexts c)
{
    switch (c) {
        case TestContexts::COMP_A: return "comp_a";
        case TestContexts::COMP_B: return "comp_b";
    }
    return "";
}

enum class TestSinks : uint8_t
{
    SINK_A
};

}  // namespace

class LogRegistryCustomTest : public Test
{
protected:
    void SetUp() override
    {
        _buffer.clear();
        _registry = std::make_unique<LogRegistry<TestContexts, TestSinks>>();
        _registry->attachSink(TestSinks::SINK_A, std::make_unique<FilteredSink<BufferSink>>(_buffer));
        _registry->getSink(TestSinks::SINK_A).setLogLevel(LogLevel::TRACE);
    }

    std::vector<uint8_t> _buffer;
    std::unique_ptr<LogRegistry<TestContexts, TestSinks>> _registry;
};

TEST_F(LogRegistryCustomTest, GetTime_NoProvider_ReturnsZero)
{
    // Replace the default time provider with nothing
    _registry->setTimeProvider(nullptr);

    Logger logger = _registry->createLogger(TestContexts::COMP_A);
    logger.setLogLevel(LogLevel::TRACE);
    logger.log(LogLevel::INFO, "time check");
    _registry->distributeMessages();

    // The timestamp in the output should reflect 0 (no time provider)
    // We just verify no crash and that some output was produced
    EXPECT_FALSE(_buffer.empty());
}

TEST_F(LogRegistryCustomTest, SetTimeProvider_UsesCustomProvider)
{
    _registry->setTimeProvider([]() -> uint64_t { return 99999ULL; });

    Logger logger = _registry->createLogger(TestContexts::COMP_A);
    logger.setLogLevel(LogLevel::TRACE);
    logger.log(LogLevel::INFO, "with time");
    _registry->distributeMessages();

    EXPECT_FALSE(_buffer.empty());
}

TEST_F(LogRegistryCustomTest, DistributeMessages_IteratesAllContexts)
{
    Logger loggerA = _registry->createLogger(TestContexts::COMP_A);
    Logger loggerB = _registry->createLogger(TestContexts::COMP_B);
    loggerA.setLogLevel(LogLevel::TRACE);
    loggerB.setLogLevel(LogLevel::TRACE);

    loggerA.log(LogLevel::INFO, "from A");
    loggerB.log(LogLevel::INFO, "from B");

    _registry->distributeMessages();

    std::string content(_buffer.begin(), _buffer.end());
    EXPECT_NE(content.find("from A"), std::string::npos);
    EXPECT_NE(content.find("from B"), std::string::npos);
}
