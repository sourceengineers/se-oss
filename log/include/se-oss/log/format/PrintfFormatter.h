/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "StringBuffer.h"

namespace se_oss {

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
    static size_t format(
        void* buffer,
        std::size_t bufferSize,
        const LogRecord& record,
        const char* const formatString,
        const Values&... values
    )
    {
        StringBuffer string {buffer, bufferSize};
        string.appendTime<TIME_STRING>(record.timestamp);
        string.append("%s ", toShortString(record.metadata.level));
        string.append("[%s] -- ", record.loggerName);
        string.append(formatString, std::forward<const Values>(values)...);
        string.append("\n");
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
}  // namespace se_oss
