/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */


#include "SinkMock.h"
#include "se-oss/log/sink/FilteredSink.h"

#include <gtest/gtest.h>


using namespace se_oss;
using namespace testing;

class FilteredSinkTest : public Test
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

    void callSink(LogLevel filter, LogLevel logLevel, bool expectCall)
    {
        LogMetadata metadata {};
        metadata.level = logLevel;
        _filteredSink.setLogLevel(filter);
        EXPECT_CALL(_filteredSink.inner(), write(_,_)).Times(expectCall ? 1 : 0);
        _filteredSink.write(metadata, nullptr, 0U);
        Mock::VerifyAndClearExpectations(&_filteredSink.inner());
    }

    FilteredSink<SinkMock> _filteredSink {};
};



TEST_F(FilteredSinkTest, FilterTrace)
{
    LogLevel filterLevel {LogLevel::TRACE};
    callSink(filterLevel, LogLevel::TRACE, true);
    callSink(filterLevel, LogLevel::DEBUG, true);
    callSink(filterLevel, LogLevel::INFO, true);
    callSink(filterLevel, LogLevel::WARN, true);
    callSink(filterLevel, LogLevel::ERROR, true);
    callSink(filterLevel, LogLevel::FATAL, true);
}

TEST_F(FilteredSinkTest, FilterDebug)
{
    LogLevel filterLevel {LogLevel::DEBUG};
    callSink(filterLevel, LogLevel::TRACE, false);
    callSink(filterLevel, LogLevel::DEBUG, true);
    callSink(filterLevel, LogLevel::INFO, true);
    callSink(filterLevel, LogLevel::WARN, true);
    callSink(filterLevel, LogLevel::ERROR, true);
    callSink(filterLevel, LogLevel::FATAL, true);
}

TEST_F(FilteredSinkTest, FilterInfo)
{
    LogLevel filterLevel {LogLevel::INFO};
    callSink(filterLevel, LogLevel::TRACE, false);
    callSink(filterLevel, LogLevel::DEBUG, false);
    callSink(filterLevel, LogLevel::INFO, true);
    callSink(filterLevel, LogLevel::WARN, true);
    callSink(filterLevel, LogLevel::ERROR, true);
    callSink(filterLevel, LogLevel::FATAL, true);
}

TEST_F(FilteredSinkTest, FilterWarn)
{
    LogLevel filterLevel {LogLevel::WARN};
    callSink(filterLevel, LogLevel::TRACE, false);
    callSink(filterLevel, LogLevel::DEBUG, false);
    callSink(filterLevel, LogLevel::INFO, false);
    callSink(filterLevel, LogLevel::WARN, true);
    callSink(filterLevel, LogLevel::ERROR, true);
    callSink(filterLevel, LogLevel::FATAL, true);
}

TEST_F(FilteredSinkTest, FilterError)
{
    LogLevel filterLevel {LogLevel::ERROR};
    callSink(filterLevel, LogLevel::TRACE, false);
    callSink(filterLevel, LogLevel::DEBUG, false);
    callSink(filterLevel, LogLevel::INFO, false);
    callSink(filterLevel, LogLevel::WARN, false);
    callSink(filterLevel, LogLevel::ERROR, true);
    callSink(filterLevel, LogLevel::FATAL, true);
}

TEST_F(FilteredSinkTest, FilterFatal)
{
    LogLevel filterLevel {LogLevel::FATAL};
    callSink(filterLevel, LogLevel::TRACE, false);
    callSink(filterLevel, LogLevel::DEBUG, false);
    callSink(filterLevel, LogLevel::INFO, false);
    callSink(filterLevel, LogLevel::WARN, false);
    callSink(filterLevel, LogLevel::ERROR, false);
    callSink(filterLevel, LogLevel::FATAL, true);
}

TEST_F(FilteredSinkTest, FilterOff)
{
    LogLevel filterLevel {LogLevel::OFF};
    callSink(filterLevel, LogLevel::TRACE, false);
    callSink(filterLevel, LogLevel::DEBUG, false);
    callSink(filterLevel, LogLevel::INFO, false);
    callSink(filterLevel, LogLevel::WARN, false);
    callSink(filterLevel, LogLevel::ERROR, false);
    callSink(filterLevel, LogLevel::FATAL, false);
}

TEST_F(FilteredSinkTest, CustomFilter)
{
    LogMetadata metadata {};
    _filteredSink.setFilter([](const auto& metadata) -> bool {
        return metadata.sourceId == 42;
    });

    metadata.sourceId = 1;
    EXPECT_CALL(_filteredSink.inner(), write(_,_)).Times(0);
    _filteredSink.write(metadata, nullptr, 0U);
    Mock::VerifyAndClearExpectations(&_filteredSink.inner());

    metadata.sourceId = 42;
    EXPECT_CALL(_filteredSink.inner(), write(_,_)).Times(1);
    _filteredSink.write(metadata, nullptr, 1U);
    Mock::VerifyAndClearExpectations(&_filteredSink.inner());
}

TEST_F(FilteredSinkTest, Flush)
{
    EXPECT_CALL(_filteredSink.inner(), flush()).Times(1);
    _filteredSink.flush();
}
