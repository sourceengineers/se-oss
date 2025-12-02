/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_LOG_TYPES_H
#define SE_LOG_TYPES_H

#include <array>
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

constexpr  uint8_t toUint(LogLevel level)
{
    return static_cast<uint8_t>(level);
}

struct LogMetadata
{
    LogLevel logLevel {LogLevel::TRACE};
    uint8_t sourceId {UINT8_MAX};
    uint16_t tag {UINT16_MAX};
};

struct LogRecord
{
    LogMetadata metadata {};
    const char* sourceName {nullptr};
    uint64_t timestamp {UINT64_MAX};
};

} // namespace se

#endif // SE_LOG_TYPES_H
