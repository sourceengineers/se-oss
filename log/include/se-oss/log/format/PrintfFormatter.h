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
                                                      _capacity {size - END_LINE_LENGTH} {}

    ~LogStringBuffer() = default;
    LogStringBuffer(const LogStringBuffer&) = delete;
    LogStringBuffer(LogStringBuffer&&) = delete;
    LogStringBuffer& operator=(const LogStringBuffer&) = delete;
    LogStringBuffer& operator=(LogStringBuffer&&) = delete;

    void append(const char* const string)
    {
        if (!valid || string == nullptr) {
            return;
        }
        std::size_t copyLength = std::min(std::strlen(string), _capacity - _length);
        std::copy_n(string, copyLength, _buffer + _length);
        _length += copyLength;
        _buffer[_length] = '\0';
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

    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::NONE> appendTime(uint64_t) const
    {
        (void)this;
    }

    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::DECIMAL> appendTime(uint64_t timestamp)
    {
        append("%" PRIu64 " ", timestamp);
    }

    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::DECIMAL_8> appendTime(uint64_t timestamp)
    {
        static constexpr std::uint64_t TIEMSTAMP_WRAP_VALUE {100000000ULL};
        if (timestamp >= TIEMSTAMP_WRAP_VALUE) {
            timestamp = timestamp % TIEMSTAMP_WRAP_VALUE;
        }
        append("%08" PRIu32 " ", static_cast<uint32_t>(timestamp));
    }

    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::DECIMAL_10> appendTime(uint64_t timestamp)
    {
        static constexpr std::uint64_t TIEMSTAMP_WRAP_VALUE {10000000000ULL};
        if (timestamp >= TIEMSTAMP_WRAP_VALUE) {
            timestamp = timestamp % TIEMSTAMP_WRAP_VALUE;
        }
        append("%010" PRIu64 " ", timestamp);
    }

    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::HEX> appendTime(uint64_t timestamp)
    {
        append("%" PRIX64 " ", timestamp);
    }

    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::HEX_8> appendTime(uint64_t timestamp)
    {
        append("%08" PRIX32 " ", static_cast<uint32_t>(timestamp));
    }

    /**
     * Converts an epoch timestamp to ISO8601 time string
     *
     * @param timestamp Timestamp assumed to be microseconds since epoch
     */
    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::ISO8601> appendTime(uint64_t timestamp)
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
        std::size_t length = std::strftime(_buffer + _length, _capacity - _length, format, time);
        if (length == 0) {
            valid = false;
        } else {
            _length += length;
        }
        append(".%03" PRIu32 "Z ", timestampMilliseconds % MILLISECONDS_PER_SECOND);
    }

    void endLine()
    {
        if (_length > _capacity) {
            return;
        }
        _buffer[_length] = END_LINE;
        _length++;
    }

    std::size_t length() const
    {
        return valid ? _length : 0U;
    }

private:
    static constexpr std::size_t END_LINE_LENGTH {1U};
    static constexpr char END_LINE {'\n'};

    char* _buffer;
    std::size_t _capacity;
    std::size_t _length {0U};
    bool valid {true};
};

/**
 * Formatter that formats log records into human-readable strings using printf-style formatting.
 */
template<TimeFormat TIME_STRING>
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
        string.appendTime<TIME_STRING>(record.timestamp);
        string.append("%s ", toShortString(record.metadata.level));
        string.append("[%s] -- ", record.sourceName);
        string.append(formatString, std::forward<const Values>(values)...);
        string.endLine();
        return string.length();
    }

    /**
     * Unsupported implementation for resource ID based formatting.
     *
     * @tparam Values types of the values.
     * @param buffer The buffer to write into.
     * @param bufferSize The available size.
     * @return The number of bytes written, or 0 on failure.
     */
    template<typename... Values>
    static size_t format(void* buffer, std::size_t bufferSize, const LogRecord&, uint32_t, const Values&...)
    {
        if (bufferSize < sizeof(FEATURE_NOT_SUPPORTED_MESSAGE)) {
            return 0U;
        }

        char* stringBuffer = static_cast<char*>(buffer);
        std::copy_n(FEATURE_NOT_SUPPORTED_MESSAGE, sizeof(FEATURE_NOT_SUPPORTED_MESSAGE), stringBuffer);
        return sizeof(FEATURE_NOT_SUPPORTED_MESSAGE);
    }

private:
    static constexpr char FEATURE_NOT_SUPPORTED_MESSAGE[] {"Printf formatting does not support string replacement\n"};
};
} // namespace se_oss_oss
