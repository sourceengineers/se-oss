/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cinttypes>
#include <cstdio>

namespace se_oss {

// todo: template for time and other format stuff
class PrintfFormatter
{
public:
    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, uint32_t formatStringId, const Values& ...values)
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

    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString, const Values& ...values)
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

    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString)
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
} // namespace se_oss
