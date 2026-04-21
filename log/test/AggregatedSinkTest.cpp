/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "LogSinkMock.h"
#include "se-oss/log/sink/AggregatedSink.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

using namespace se_oss;
using namespace testing;

namespace {

enum class TestSinks : uint8_t
{
    SINK_A,
    SINK_B,
    SINK_C
};

}  // namespace

class AggregatedSinkTest : public Test
{
protected:
    void SetUp() override
    {
        auto sinkA = std::make_unique<LogSinkMock>();
        auto sinkB = std::make_unique<LogSinkMock>();

        _sinkA = sinkA.get();
        _sinkB = sinkB.get();

        _aggregatedSink.attachSink(TestSinks::SINK_A, std::move(sinkA));
        _aggregatedSink.attachSink(TestSinks::SINK_B, std::move(sinkB));
    }

    AggregatedSink<TestSinks> _aggregatedSink {};
    LogSinkMock* _sinkA {nullptr};
    LogSinkMock* _sinkB {nullptr};
};

TEST_F(AggregatedSinkTest, EmptyByDefault)
{
    AggregatedSink<TestSinks> emptySink;
    EXPECT_TRUE(emptySink.empty());
}

TEST_F(AggregatedSinkTest, NotEmptyAfterAttach) { EXPECT_FALSE(_aggregatedSink.empty()); }

TEST_F(AggregatedSinkTest, WriteForwardsToAllSinks)
{
    LogMetadata metadata {};
    metadata.level = LogLevel::INFO;
    uint8_t data[] = {1, 2, 3};

    EXPECT_CALL(*_sinkA, write(_, _, 3)).Times(1);
    EXPECT_CALL(*_sinkB, write(_, _, 3)).Times(1);

    _aggregatedSink.write(metadata, data, sizeof(data));
}

TEST_F(AggregatedSinkTest, FlushForwardsToAllSinks)
{
    EXPECT_CALL(*_sinkA, flush()).Times(1);
    EXPECT_CALL(*_sinkB, flush()).Times(1);

    _aggregatedSink.flush();
}

TEST_F(AggregatedSinkTest, SetLogLevelForwardsToAllSinks)
{
    EXPECT_CALL(*_sinkA, setLogLevel(LogLevel::WARN)).Times(1);
    EXPECT_CALL(*_sinkB, setLogLevel(LogLevel::WARN)).Times(1);

    _aggregatedSink.setLogLevel(LogLevel::WARN);
}

TEST_F(AggregatedSinkTest, SetFilterForwardsToAllSinks)
{
    EXPECT_CALL(*_sinkA, setFilter(_)).Times(1);
    EXPECT_CALL(*_sinkB, setFilter(_)).Times(1);

    _aggregatedSink.setFilter([](const LogMetadata& metadata) -> bool { return metadata.contextTag == 1; });
}

TEST_F(AggregatedSinkTest, GetSinkReturnsCorrectSink)
{
    ILogSink& retrievedA = _aggregatedSink.getSink(TestSinks::SINK_A);
    ILogSink& retrievedB = _aggregatedSink.getSink(TestSinks::SINK_B);

    EXPECT_EQ(&retrievedA, _sinkA);
    EXPECT_EQ(&retrievedB, _sinkB);
}

TEST_F(AggregatedSinkTest, GetSinkThrowsForUnknownId)
{
    EXPECT_THROW(_aggregatedSink.getSink(TestSinks::SINK_C), std::out_of_range);
}

TEST_F(AggregatedSinkTest, WriteWithNoSinksDoesNotCrash)
{
    AggregatedSink<TestSinks> emptySink;
    LogMetadata metadata {};

    EXPECT_NO_THROW(emptySink.write(metadata, nullptr, 0));
}

TEST_F(AggregatedSinkTest, FlushWithNoSinksDoesNotCrash)
{
    AggregatedSink<TestSinks> emptySink;

    EXPECT_NO_THROW(emptySink.flush());
}

TEST_F(AggregatedSinkTest, SetFilter_ThenInvoke_DelegatesToFilter)
{
    // Capture the filter function that gets forwarded to sinks
    LogFilterFunction capturedFilter;
    EXPECT_CALL(*_sinkA, setFilter(_)).WillOnce(SaveArg<0>(&capturedFilter));
    EXPECT_CALL(*_sinkB, setFilter(_)).Times(1);

    _aggregatedSink.setFilter([](const LogMetadata& m) -> bool { return m.level >= LogLevel::WARN; });

    // Invoke the captured internal lambda — _filter is non-null, delegates
    LogMetadata metadata {};
    metadata.level = LogLevel::WARN;
    EXPECT_TRUE(capturedFilter(metadata));

    metadata.level = LogLevel::DEBUG;
    EXPECT_FALSE(capturedFilter(metadata));
}

TEST_F(AggregatedSinkTest, SetLogLevel_ClearsFilter_ThenSetFilter_NullpathPath)
{
    // First set a filter
    LogFilterFunction capturedFilter;
    EXPECT_CALL(*_sinkA, setFilter(_)).WillOnce(SaveArg<0>(&capturedFilter));
    EXPECT_CALL(*_sinkB, setFilter(_)).Times(1);
    _aggregatedSink.setFilter([](const LogMetadata&) -> bool { return true; });

    // Now setLogLevel clears _filter to nullptr
    EXPECT_CALL(*_sinkA, setLogLevel(LogLevel::WARN)).Times(1);
    EXPECT_CALL(*_sinkB, setLogLevel(LogLevel::WARN)).Times(1);
    _aggregatedSink.setLogLevel(LogLevel::WARN);

    // The previously captured lambda should now see _filter == nullptr → return false
    LogMetadata metadata {};
    metadata.level = LogLevel::FATAL;
    EXPECT_FALSE(capturedFilter(metadata));
}
