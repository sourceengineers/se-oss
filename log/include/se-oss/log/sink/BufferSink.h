/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IWriter.h"
#include <vector>
#include <cstdint>

namespace se_oss {

/**
 * Sink that writes log messages to an in-memory buffer.
 *
 * Useful for testing or capturing logs programmatically.
 */
class BufferSink final : public IWriter
{
public:
    /**
     * Constructs a BufferSink that writes to the provided buffer.
     * @param buffer Reference to the vector that will store the log data.
     */
    explicit BufferSink(std::vector<uint8_t>& buffer)
        : _buffer(buffer)
    {
    }

    ~BufferSink() override = default;
    BufferSink(const BufferSink&) = delete;
    BufferSink(BufferSink&&) = delete;
    BufferSink& operator=(const BufferSink&) = delete;
    BufferSink& operator=(BufferSink&&) = delete;

    void write(const void* data, std::size_t length) override
    {
        if (data != nullptr && length > 0) {
            const auto* byteData = static_cast<const uint8_t*>(data);
            _buffer.insert(_buffer.end(), byteData, byteData + length);
        }
    }

    void flush() override {}

private:
    std::vector<uint8_t>& _buffer;
};

} // namespace se_oss_oss
