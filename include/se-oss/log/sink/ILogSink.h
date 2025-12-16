/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_LOG_ISINK_H
#define SE_LOG_ISINK_H

#include "se-oss/log/Types.h"

#include <cstring>

namespace se_oss {

class ILogSink
{
protected:
    ILogSink() = default;
public:
    virtual ~ILogSink() = default;
    ILogSink(const ILogSink&) = delete;
    ILogSink(ILogSink&&) = delete;
    ILogSink& operator=(const ILogSink&) = delete;
    ILogSink& operator=(ILogSink&&) = delete;

    virtual void write(LogMetadata metadata, const void* data, std::size_t length) = 0;
    virtual void flush() = 0;

    virtual void setLogLevel(LogLevel level) = 0;
    virtual void setComponentLogLevel(LogLevel level, uint8_t componentId) = 0;
};

struct LogHeader
{
    static constexpr size_t PACKED_SIZE {6U};
    LogMetadata metadata {};
    uint16_t messageLength {0U};
};

constexpr void* serialize(const LogHeader& header, void* buffer, std::size_t bufferSize)
{
    if (buffer == nullptr || bufferSize < LogHeader::PACKED_SIZE) {
        return nullptr;
    }

    auto* byteBuffer = static_cast<uint8_t*>(buffer);
    std::memcpy(byteBuffer, &header.metadata.logLevel, sizeof(header.metadata.logLevel));
    byteBuffer += sizeof(header.metadata.logLevel);
    std::memcpy(byteBuffer, &header.metadata.sourceId, sizeof(header.metadata.sourceId));
    byteBuffer += sizeof(header.metadata.sourceId);
    std::memcpy(byteBuffer, &header.metadata.tag, sizeof(header.metadata.tag));
    byteBuffer += sizeof(header.metadata.tag);
    std::memcpy(byteBuffer, &header.messageLength, sizeof(header.messageLength));
    byteBuffer += sizeof(header.messageLength);
    return byteBuffer;
}

constexpr const void* deserialize(LogHeader& header, const void* buffer, std::size_t bufferSize)
{
    if (buffer == nullptr || bufferSize < LogHeader::PACKED_SIZE) {
        return nullptr;
    }
    auto* byteBuffer = static_cast<const uint8_t*>(buffer);
    std::memcpy(&header.metadata.logLevel, buffer, sizeof(header.metadata.logLevel));
    byteBuffer += sizeof(header.metadata.logLevel);
    std::memcpy(&header.metadata.sourceId, byteBuffer, sizeof(header.metadata.sourceId));
    byteBuffer += sizeof(header.metadata.sourceId);
    std::memcpy(&header.metadata.tag, byteBuffer, sizeof(header.metadata.tag));
    byteBuffer += sizeof(header.metadata.tag);
    std::memcpy(&header.messageLength, byteBuffer, sizeof(header.messageLength));
    byteBuffer += sizeof(header.messageLength);
    return byteBuffer;
}


} // namespace se

#endif // SE_LOG_ISINK_H
