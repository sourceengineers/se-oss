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
    TRACE = 0,  //!< Detailed trace information.
    DEBUG = 1,  //!< Debugging information.
    INFO = 2,  //!< General information.
    WARN = 3,  //!< Warning conditions.
    ERROR = 4,  //!< Error conditions.
    FATAL = 5,  //!< Fatal errors causing termination.
    OFF = UINT8_MAX  //!< Logging disabled.
};

constexpr const char* toString(LogLevel level)
{
    switch (level) {
        case LogLevel::TRACE: return "trace";
        case LogLevel::DEBUG: return "debug";
        case LogLevel::INFO: return "info";
        case LogLevel::WARN: return "warn";
        case LogLevel::ERROR: return "error";
        case LogLevel::FATAL: return "fatal";
        case LogLevel::OFF: return "off";
    }
    return "";
}

constexpr const char* toShortString(LogLevel level)
{
    switch (level) {
        case LogLevel::TRACE: return "T";
        case LogLevel::DEBUG: return "D";
        case LogLevel::INFO: return "I";
        case LogLevel::WARN: return "W";
        case LogLevel::ERROR: return "E";
        case LogLevel::FATAL: return "F";
        case LogLevel::OFF: return "O";
    }
    return "";
}

constexpr uint8_t toUint(LogLevel level) { return static_cast<uint8_t>(level); }

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
    uint8_t sourceId {UINT8_MAX}; /**< The ID of the logger source. */
};

/**
 * A complete log record, including metadata and payload details.
 */
struct LogRecord
{
    LogMetadata metadata {}; /**< The log metadata. */
    const char* sourceName {nullptr}; /**< The name of the logger source. */
    uint64_t timestamp {UINT64_MAX}; /**< The timestamp of the log message. */
};

/**
 * Time formatting options
 */
enum class TimeFormat : uint8_t
{
    //! Omit timestamp.
    NONE,
    //! Timestamp in decimal format, e.g., 1768902731209138.
    DECIMAL,
    //! Trailing 8 timestamp digits in decimal format, e.g., 31209138. Note: This will fit into a u32 for devices with
    //! limited formatting support.
    DECIMAL_8,
    //! Trailing 10 timestamp digits in decimal format, e.g., 2731209138.
    DECIMAL_10,
    //! Timestamp in hex format, e.g.,  648CEC2EF99B2.
    HEX,
    //! Timestamp in hex format, e.g.,  C2EF99B2. Note: This will fit into a u32 for devices with limited formatting
    //! support.
    HEX_8,
    //! Timestamp in human-readable date time format, e.g., 2026-01-20T09:52:11.209Z. Note: Assumes that the timestamp
    is in microseconds since epoch.ISO8601

};

}  // namespace se_oss
