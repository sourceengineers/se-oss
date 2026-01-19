/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/Types.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>

namespace se_oss {

// todo: template for time and other format stuff
/**
 * Formatter that formats log records into human-readable strings using printf-style formatting.
 */
class PrintfFormatter
{
public:
    /**
     * Formats a log record using a resource ID.
     *
     * Note: Arguments are not supported for this overload in the current implementation.
     *
     * @tparam Values types of the values.
     * @param buffer The buffer to write into.
     * @param bufferSize The available size.
     * @param record The log record.
     * @param formatStringId The ID (resource ID) of the format string.
     * @return The number of bytes written, or 0 on failure.
     */
    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, const LogRecord& record, uint32_t formatStringId, const Values& ...) const
    {
        (void) this;
        char* stringBuffer = static_cast<char*>(buffer);

        // todo: format timestamp -> is set to u32 here because u64 cannot be formatted with libnano
        int32_t metaInfoLength = std::snprintf(stringBuffer, bufferSize, "%08" PRIX32 " [%s] %s ", static_cast<uint32_t>(record.timestamp), record.sourceName, toString(record.metadata.level));
        if (metaInfoLength < 0) {
            return 0U;
        }

        int32_t messageLength = std::snprintf(stringBuffer + metaInfoLength, bufferSize - metaInfoLength, "string id 0x%" PRIX32 " (arguments not supported)\n", formatStringId);
        size_t overallLength = metaInfoLength + messageLength + 1U;
        if (messageLength < 0 || overallLength > bufferSize) {
            return 0U;
        }

        return overallLength;
    }

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
    size_t format(void* buffer, std::size_t bufferSize, const LogRecord& record, const char *const formatString, const Values& ...values) const
    {
        (void) this;
        char* stringBuffer = static_cast<char*>(buffer);

        // todo: format timestamp -> is set to u32 here because u64 cannot be formatted with libnano
        int32_t metaInfoLength = std::snprintf(stringBuffer, bufferSize, "%08" PRIX32 " [%s] %s ", static_cast<uint32_t>(record.timestamp), record.sourceName, toString(record.metadata.level));
        if (metaInfoLength < 0) {
            return 0U;
        }

        int32_t messageLength = std::snprintf(stringBuffer + metaInfoLength, bufferSize - metaInfoLength, formatString, values...);
        size_t overallLength = metaInfoLength + messageLength + 2U;
        if (messageLength < 0 || overallLength > bufferSize) {
            return 0U;
        }

        *(stringBuffer + overallLength - 2U) = '\n';
        *(stringBuffer + overallLength - 1U) = '\0';

        return overallLength;
    }

    /**
     * Formats a log record using a plain string (no arguments).
     *
     * @param buffer The buffer to write into.
     * @param bufferSize The available size.
     * @param record The log record.
     * @param formatString The message string.
     * @return The number of bytes written, or 0 on failure.
     */
    size_t format(void* buffer, std::size_t bufferSize, const LogRecord& record, const char *const formatString) const
    {
        (void) this;
        char* stringBuffer = static_cast<char*>(buffer);
        // todo: format timestamp -> is set to u32 here because u64 cannot be formatted with libnano
        int32_t metaInfoLength = std::snprintf(stringBuffer, bufferSize, "%08" PRIX32 " [%s] %s ", static_cast<uint32_t>(record.timestamp), record.sourceName, toString(record.metadata.level));
        std::size_t messageLength = std::strlen(formatString);

        size_t overallLength = metaInfoLength + messageLength + 2U;
        if (metaInfoLength < 0 || overallLength > bufferSize) {
            return 0U;
        }
        std::strncat(stringBuffer, formatString, bufferSize - metaInfoLength);

        *(stringBuffer + overallLength - 2U) = '\n';
        *(stringBuffer + overallLength - 1U) = '\0';

        return overallLength;
    }
};
;
} // namespace se_oss_oss
