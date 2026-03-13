/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

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

class LogSinkMock : public ILogSink
{
public:
    MOCK_METHOD(void, write, (const LogMetadata&, const void*, std::size_t), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, setLogLevel, (LogLevel), (override));
    MOCK_METHOD(void, setFilter, (std::function<bool(const LogMetadata&)>), (override));
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

TEST_F(AggregatedSinkTest, NotEmptyAfterAttach)
{
    EXPECT_FALSE(_aggregatedSink.empty());
}

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
