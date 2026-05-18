/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/Types.h"

#include <algorithm>
#include <cinttypes>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <ctime>

namespace se_oss {

/**
 * Helper class to create and manipulate strings in a given buffer
 */
class StringBuffer
{
public:
    /**
     * Create a string buffer from a given buffer.
     *
     * @param buffer Pointer to the output buffer
     * @param size Size of the buffer in bytes
     */
    StringBuffer(void* buffer, std::size_t size) :
        _buffer {static_cast<char*>(buffer)},
        _capacity {size}
    {
        if (_buffer != nullptr) {
            _buffer[0] = TERMINATION_CHARACTER;
        } else {
            _valid = false;
        }
    }

    ~StringBuffer() = default;
    StringBuffer(const StringBuffer&) = delete;
    StringBuffer(StringBuffer&&) = delete;
    StringBuffer& operator=(const StringBuffer&) = delete;
    StringBuffer& operator=(StringBuffer&&) = delete;

    void append(const char* const string)
    {
        if (!_valid || string == nullptr) {
            return;
        }
        std::size_t copyLength = std::min(std::strlen(string), _capacity - _length - TERMINATION_LENGTH);
        std::copy_n(string, copyLength, _buffer + _length);
        _length += copyLength;
        _buffer[_length] = TERMINATION_CHARACTER;
    }

    template<typename... Values>
    void append(const char* const formatString, const Values&... values)
    {
        if (!_valid) {
            return;
        }
        int32_t length = std::snprintf(_buffer + _length, _capacity - _length, formatString, values...);
        if (length < 0) {
            _valid = false;
            _buffer[_length] = TERMINATION_CHARACTER;
        } else if (_length + length >= _capacity) {
            // snprintf truncated the string in _buffer but returned the length as if it was formatted successfully
            // So, we need to truncate the length manually.
            _length = _capacity - TERMINATION_LENGTH;
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
        static constexpr std::uint64_t TIMESTAMP_WRAP_VALUE {100000000ULL};
        if (timestamp >= TIMESTAMP_WRAP_VALUE) {
            timestamp = timestamp % TIMESTAMP_WRAP_VALUE;
        }
        append("%08" PRIu32 " ", static_cast<uint32_t>(timestamp));
    }

    template<TimeFormat TS>
    std::enable_if_t<TS == TimeFormat::DECIMAL_10> appendTime(uint64_t timestamp)
    {
        static constexpr std::uint64_t TIMESTAMP_WRAP_VALUE {10000000000ULL};
        if (timestamp >= TIMESTAMP_WRAP_VALUE) {
            timestamp = timestamp % TIMESTAMP_WRAP_VALUE;
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
        appendTime("%Y-%m-%dT%H:%M:%S", timestamp / MICROSECONDS_PER_MILLISECOND);
    }

    void appendTime(const char* format, uint64_t timestampMilliseconds)
    {
        static constexpr std::uint64_t MILLISECONDS_PER_SECOND {1000ULL};
        if (!_valid) {
            return;
        }
        auto epochSeconds = static_cast<time_t>(timestampMilliseconds / MILLISECONDS_PER_SECOND);
        tm time {};
        if (gmtime_r(&epochSeconds, &time) != &time) {
            _valid = false;
            _buffer[_length] = TERMINATION_CHARACTER;
            return;
        }

        std::size_t length = std::strftime(_buffer + _length, _capacity - _length, format, &time);
        if (length == 0) {
            _valid = false;
            _buffer[_length] = TERMINATION_CHARACTER;
        } else {
            _length += length;
        }
        // Note: a static cast is required so that the templated parameter has a fixed type.
        // Otherwise, some compilers will optimize the value out altogether.
        append(".%03" PRIu32 "Z ", static_cast<uint32_t>(timestampMilliseconds % MILLISECONDS_PER_SECOND));
    }

    std::size_t length() const { return _valid ? _length : 0U; }

private:
    static constexpr std::size_t TERMINATION_LENGTH {1U};
    static constexpr char TERMINATION_CHARACTER {'\0'};

    char* _buffer;  //!< String buffer containing a valid c-string, i.e., including the termination character
    std::size_t _capacity;  //!< Capacity of the buffer in bytes
    std::size_t _length {0U};  //!< String length not including the termination character
    bool _valid {true};
};

}  // namespace se_oss
