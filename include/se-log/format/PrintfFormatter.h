/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_FORMAT_PRINTFFORMATTER_H
#define SE_SE_LOG_FORMAT_PRINTFFORMATTER_H

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
        int32_t messageLength = std::snprintf(stringBuffer, bufferSize, "%08" PRIX32 " [%s] %s ", static_cast<uint32_t>(record.timestamp), record.sourceName, toString(record.metadata.logLevel));
        if (messageLength < 0) {
            return 0U;
        }

        return std::snprintf(stringBuffer + messageLength, bufferSize - messageLength, "string id 0x%" PRIX32 " (arguments not supported)\n", formatStringId) + messageLength + 1U;
    }

    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString, const Values& ...values)
    {
        (void) this;
        char* stringBuffer = static_cast<char*>(buffer);

        // todo: format timestamp -> is set to u32 here because u64 cannot be formatted with libnano
        int32_t messageLength = std::snprintf(stringBuffer, bufferSize, "%08" PRIX32 " [%s] %s ", static_cast<uint32_t>(record.timestamp), record.sourceName, toString(record.metadata.logLevel));
        if (messageLength < 0) {
            return 0U;
        }

        return std::snprintf(stringBuffer + messageLength, bufferSize - messageLength, formatString, values...) + messageLength + 1U;
    }

    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString)
    {
        (void) this;
        char* stringBuffer = static_cast<char*>(buffer);
        // todo: format timestamp -> is set to u32 here because u64 cannot be formatted with libnano
        int32_t messageLength = std::snprintf(stringBuffer, bufferSize, "%08" PRIu32 " [%s] %s ", static_cast<uint32_t>(record.timestamp), record.sourceName, toString(record.metadata.logLevel));
        std::size_t formatLength = std::strlen(formatString);

        if (messageLength < 0 || formatLength > bufferSize - messageLength) {
            return 0U;
        }
        std::strncat(stringBuffer, formatString, bufferSize - messageLength);
        return messageLength + formatLength + 1U;
    }
};
;
} // namespace se_oss

#endif // SE_SE_LOG_FORMAT_PRINTFFORMATTER_H