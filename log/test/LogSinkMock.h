/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/sink/ILogSink.h"

#include <gmock/gmock.h>

namespace se_oss {

class LogSinkMock : public se_oss::ILogSink
{
public:
    MOCK_METHOD(void, write, (const LogMetadata&, const void*, std::size_t), (override));
    MOCK_METHOD(void, flush, (), (override));
    MOCK_METHOD(void, setLogLevel, (LogLevel), (override));
    MOCK_METHOD(void, setFilter, (LogFilterFunction), (override));
};

}