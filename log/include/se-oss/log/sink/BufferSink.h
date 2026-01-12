/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/IWriter.h"
#include <vector>
#include <cstdint>

namespace se_oss {

class BufferSink final : public IWriter
{
public:
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

} // namespace se_oss
