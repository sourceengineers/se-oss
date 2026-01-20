/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/Types.h"

#include <algorithm>
#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace se_oss {
class LogStringBuffer
{
public:
    LogStringBuffer(void* buffer, std::size_t size) : _buffer {static_cast<char*>(buffer)},
                                                      _capacity {size - TERMINATION_LENGTH} {}

    ~LogStringBuffer() = default;
    LogStringBuffer(const LogStringBuffer&) = delete;
    LogStringBuffer(LogStringBuffer&&) = delete;
    LogStringBuffer& operator=(const LogStringBuffer&) = delete;
    LogStringBuffer& operator=(LogStringBuffer&&) = delete;

    void append(const char* const string)
    {
        if (!valid || string == nullptr || _length + std::strlen(string) >= _capacity) {
            return;
        }
        std::copy_n(string, std::strlen(string) + TERMINATION_LENGTH, _buffer + _length);
        _length += std::strlen(string);
    }

    template<typename... Values>
    void append(const char* const formatString, const Values&... values)
    {
        if (!valid) {
            return;
        }
        int32_t length = std::snprintf(_buffer + _length, _capacity - _length, formatString, values...);
        if (length < 0) {
            valid = false;
        } else {
            _length += length;
        }
    }

    template<TimeString TS>
    std::enable_if_t<TS == TimeString::DECIMAL> appendTime(uint64_t timestamp)
    {
        append("%" PRIu64 " ", timestamp);
    }

    template<TimeString TS>
    std::enable_if_t<TS == TimeString::HEX> appendTime(uint64_t timestamp)
    {
        append("%08" PRIX32 " ", static_cast<uint32_t>(timestamp));
    }

    /**
     * Converts an epoch timestamp to ISO8601 time string
     *
     * @param timestamp Timestamp assumed to be microseconds since epoch
     */
    template<TimeString TS>
    std::enable_if_t<TS == TimeString::ISO8601> appendTime(uint64_t timestamp)
    {
        static constexpr std::uint64_t MICROSECONDS_PER_MILLISECOND {1000ULL};
        if (!valid) {
            return;
        }

        appendTime("%Y-%m-%dT%H:%M:%S", timestamp / MICROSECONDS_PER_MILLISECOND);
    }

    void appendTime(const char* format, uint64_t timestampMilliseconds)
    {
        static constexpr std::uint64_t MILLISECONDS_PER_SECOND {1000ULL};
        static constexpr std::uint64_t MICROSECONDS_PER_MILLISECOND {1000ULL};
        if (!valid) {
            return;
        }
        auto epochSeconds = static_cast<time_t>(timestampMilliseconds / MICROSECONDS_PER_MILLISECOND);
        const tm* time = std::gmtime(&epochSeconds);  // returns a pointer to a static object and thus does not allocate memory
        int32_t length = std::strftime(_buffer + _length, _capacity - _length, format, time);
        if (length < 0) {
            valid = false;
        } else {
            _length += length;
        }
        append(".%03Z" PRIu32 " ", timestampMilliseconds % MILLISECONDS_PER_SECOND);
    }

    /**
     * Get the string length *including* the termination character
     */
    std::size_t length() const
    {
        return valid ? _length + TERMINATION_LENGTH : 0U;
    }

private:
    static constexpr std::size_t TERMINATION_LENGTH {1U};

    char* _buffer;
    std::size_t _capacity;
    std::size_t _length {0U};
    bool valid {true};
};

/**
 * Formatter that formats log records into human-readable strings using printf-style formatting.
 */
template<TimeString TIME_STRING>
class PrintfFormatter
{
public:
    /**
     * Formats a log record using a format string.
     *
     * @tparam Values Type of arguments.
     * @param buffer The buffer to write into.
     * @param bufferSize The available size.
     * @param record The log record.
     * @param formatString The printf-style format string.
     * @param values The arguments to format.
     * @return The number of bytes written, or 0 on failure.
     */
    template<typename... Values>
    static size_t format(void* buffer, std::size_t bufferSize, const LogRecord& record, const char* const formatString, const Values&... values)
    {
        LogStringBuffer string {buffer, bufferSize};
        string.template appendTime<TIME_STRING>(record.timestamp);
        string.append("%s ", toShortString(record.metadata.level));
        string.append("[%s] -- ", record.sourceName);
        string.append(formatString, std::forward<const Values>(values)...);
        return string.length();
    }

    /**
     * Unsupported implementation for resource ID based formatting.
     *
     * @tparam Values types of the values.
     * @param buffer The buffer to write into.
     * @param bufferSize The available size.
     * @param record The log record.
     * @param formatStringId The ID (resource ID) of the format string.
     * @return The number of bytes written, or 0 on failure.
     */
    template<typename... Values>
    static size_t format(void* buffer, std::size_t bufferSize, const LogRecord& record, uint32_t formatStringId, const Values&...)
    {
        char* stringBuffer = static_cast<char*>(buffer);
        std::copy_n(FEATURE_NOT_SUPPORTED_MESSAGE, sizeof(FEATURE_NOT_SUPPORTED_MESSAGE), stringBuffer);
        return sizeof(FEATURE_NOT_SUPPORTED_MESSAGE);
    }

private:
    static constexpr char FEATURE_NOT_SUPPORTED_MESSAGE[] {"Printf formatting does not support string replacement"};
};
} // namespace se_oss_oss
