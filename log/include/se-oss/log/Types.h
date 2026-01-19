/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace se_oss {

/**
 * Enum representing the severity level of a log message.
 */
enum class LogLevel : uint8_t
{
    TRACE = 0,  /**< Detailed trace information. */
    DEBUG = 1,  /**< Debugging information. */
    INFO = 2,   /**< General information. */
    WARN = 3,   /**< Warning conditions. */
    ERROR = 4,  /**< Error conditions. */
    FATAL = 5,  /**< Fatal errors causing termination. */
    OFF = UINT8_MAX /**< Logging disabled. */
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

/**
 * Metadata associated with a log message.
 */
struct LogMetadata
{
    LogLevel level {LogLevel::TRACE}; /**< The log level. */
    uint8_t sourceId {UINT8_MAX};     /**< The ID of the logger source. */
};

/**
 * A complete log record, including metadata and payload details.
 */
struct LogRecord
{
    LogMetadata metadata {};          /**< The log metadata. */
    const char* sourceName {nullptr}; /**< The name of the logger source. */
    uint64_t timestamp {UINT64_MAX};  /**< The timestamp of the log message. */
};

} // namespace se_oss
