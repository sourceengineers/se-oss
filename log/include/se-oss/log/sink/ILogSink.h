/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
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
    static constexpr size_t PACKED_SIZE {5U}; /**< Size of the serialized header. */
    LogMetadata metadata {}; /**< Log metadata. */
    uint16_t messageLength {0U}; /**< Length of the message payload. */
};

template<typename T>
constexpr uint8_t* writeValue(const T& value, uint8_t* writer, const uint8_t* bufferEnd)
{
    if (writer == nullptr || static_cast<std::size_t>(bufferEnd - writer) < sizeof(T)) {
        return nullptr;
    }
    std::memcpy(writer, &value, sizeof(T));
    return writer + sizeof(T);
}

template<typename T>
constexpr const uint8_t* readValue(T& value, const uint8_t* reader, const uint8_t* bufferEnd)
{
    if (reader == nullptr || static_cast<std::size_t>(bufferEnd - reader) < sizeof(T)) {
        return nullptr;
    }
    std::memcpy(&value, reader, sizeof(T));
    return reader + sizeof(T);
}

constexpr void* serialize(const LogHeader& header, void* buffer, std::size_t bufferSize)
{
    if (buffer == nullptr || bufferSize < LogHeader::PACKED_SIZE) {
        return nullptr;
    }

    auto* writer = static_cast<uint8_t*>(buffer);
    const auto* bufferEnd = writer + bufferSize;
    writer = writeValue(header.metadata.level, writer, bufferEnd);
    writer = writeValue(header.metadata.contextTag, writer, bufferEnd);
    writer = writeValue(header.metadata.loggerTag, writer, bufferEnd);
    writer = writeValue(header.messageLength, writer, bufferEnd);
    return writer;
}

constexpr const void* deserialize(LogHeader& header, const void* buffer, std::size_t bufferSize)
{
    if (buffer == nullptr || bufferSize < LogHeader::PACKED_SIZE) {
        return nullptr;
    }
    auto* reader = static_cast<const uint8_t*>(buffer);
    const auto* bufferEnd = reader + bufferSize;
    reader = readValue(header.metadata.level, reader, bufferEnd);
    reader = readValue(header.metadata.contextTag, reader, bufferEnd);
    reader = readValue(header.metadata.loggerTag, reader, bufferEnd);
    reader = readValue(header.messageLength, reader, bufferEnd);
    return reader;
}

}  // namespace se_oss
