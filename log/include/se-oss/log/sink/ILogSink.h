/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IWriter.h"
#include "se-oss/log/ILogFilter.h"
#include "se-oss/log/Types.h"

#include <cstring>

namespace se_oss {

/**
 * Interface for log sinks.
 *
 * A sink is responsible for writing formatted log messages to a destination.
 * It combines filtering capabilities (via ILogFilter) with writing capabilities.
 */
class ILogSink : public ILogFilter
{
protected:
    ILogSink() = default;

public:
    ~ILogSink() override = default;
    ILogSink(const ILogSink&) = delete;
    ILogSink(ILogSink&&) = delete;
    ILogSink& operator=(const ILogSink&) = delete;
    ILogSink& operator=(ILogSink&&) = delete;

    /**
     * Writes a serialized log message.
     * @param metadata The log metadata (level, source ID).
     * @param data Pointer to the serialized message data.
     * @param data Pointer to the serialized message data.
     * @param length Length of the data in bytes.
     */
    virtual void write(const LogMetadata& metadata, const void* data, std::size_t length) = 0;

    /**
     * Flushes any buffered data to the underlying destination.
     */
    virtual void flush() = 0;

    // ILogFilter implementation
    void setLogLevel(LogLevel level) override = 0;
    void setFilter(std::function<bool(const LogMetadata&)> filter) override = 0;
};

/**
 * Header structure used when serializing messages into the log buffer in deferred mode.
 */
struct LogHeader
{
    static constexpr size_t PACKED_SIZE {4U}; /**< Size of the serialized header. */
    LogMetadata metadata {}; /**< Log metadata. */
    uint16_t messageLength {0U}; /**< Length of the message payload. */
};

constexpr void* serialize(const LogHeader& header, void* buffer, std::size_t bufferSize)
{
    if (buffer == nullptr || bufferSize < LogHeader::PACKED_SIZE) {
        return nullptr;
    }

    auto* byteBuffer = static_cast<uint8_t*>(buffer);
    std::memcpy(byteBuffer, &header.metadata.level, sizeof(header.metadata.level));
    byteBuffer += sizeof(header.metadata.level);
    std::memcpy(byteBuffer, &header.metadata.sourceId, sizeof(header.metadata.sourceId));
    byteBuffer += sizeof(header.metadata.sourceId);
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
    std::memcpy(&header.metadata.level, buffer, sizeof(header.metadata.level));
    byteBuffer += sizeof(header.metadata.level);
    std::memcpy(&header.metadata.sourceId, byteBuffer, sizeof(header.metadata.sourceId));
    byteBuffer += sizeof(header.metadata.sourceId);
    std::memcpy(&header.messageLength, byteBuffer, sizeof(header.messageLength));
    byteBuffer += sizeof(header.messageLength);
    return byteBuffer;
}

}  // namespace se_oss
