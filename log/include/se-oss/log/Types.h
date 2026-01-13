/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace se_oss {

enum class LogLevel : uint8_t
{
    TRACE = 0,
    DEBUG = 1,
    INFO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5,
    OFF = UINT8_MAX
};

constexpr const char* toString(LogLevel level)
{
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO: return "INFO ";
        case LogLevel::WARN: return "WARN ";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        case LogLevel::OFF: return "OFF  ";
    }
    return "";
}

constexpr uint8_t toUint(LogLevel level)
{
    return static_cast<uint8_t>(level);
}

constexpr LogLevel toLogLevel(uint8_t level)
{
    switch (level) {
        case toUint(LogLevel::TRACE): return LogLevel::TRACE;
        case toUint(LogLevel::DEBUG): return LogLevel::DEBUG;
        case toUint(LogLevel::INFO): return LogLevel::INFO;
        case toUint(LogLevel::WARN): return LogLevel::WARN;
        case toUint(LogLevel::ERROR): return LogLevel::ERROR;
        case toUint(LogLevel::FATAL): return LogLevel::FATAL;
        case toUint(LogLevel::OFF):  // fallthrough
        default: return LogLevel::OFF;
    }
}

struct LogMetadata
{
    LogLevel level {LogLevel::TRACE};
    uint8_t sourceId {UINT8_MAX};
};

struct LogRecord
{
    LogMetadata metadata {};
    const char* sourceName {nullptr};
    uint64_t timestamp {UINT64_MAX};
};

} // namespace se
