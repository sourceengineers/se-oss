/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IBuffer.h"
#include "se-oss/log/sink/ILogSink.h"

#include <algorithm>
#include <array>

namespace se_oss {

/**
 * Specific implementation of IBuffer for immediate logging.
 *
 * This implementation does not buffer multiple messages between threads.
 * Instead, there is space one single message which is used to format a message
 * before immediately triggering the consumer.
 *
 * @tparam MAX_MESSAGE_LENGTH The maximum size of the intermediate formatting buffer.
 */
template<std::size_t MAX_MESSAGE_LENGTH>
class ImmediateBuffer final : public IBuffer
{
public:
    ImmediateBuffer() = default;
    ~ImmediateBuffer() override = default;
    ImmediateBuffer(const ImmediateBuffer&) = delete;
    ImmediateBuffer(ImmediateBuffer&&) = delete;
    ImmediateBuffer& operator=(const ImmediateBuffer&) = delete;
    ImmediateBuffer& operator=(ImmediateBuffer&&) = delete;

    std::size_t capacity() const override { return _formatBuffer.size(); }
    std::size_t size() const override { return _size - _readPosition; }
    std::size_t free() const override { return size() == 0U ? _formatBuffer.size() : 0U; }

    WriteRegion reserveWrite(std::size_t size) override
    {
        if (size == 0U || size > free()) {
            return {nullptr, 0U};
        }
        _reservedSize = size;
        return {_formatBuffer.data(), size};
    }

    void commitWrite(std::size_t bytesWritten) override
    {
        _size = std::min(bytesWritten, _reservedSize);
        _readPosition = 0U;
        _reservedSize = 0U;
    }

    ReadRegion acquireRead() override
    {
        return size() > 0U ? ReadRegion {_formatBuffer.data() + _readPosition, size()} : EMPTY_READ_REGION;
    }

    void consumeRead(std::size_t bytesRead) override
    {
        _readPosition += std::min(bytesRead, size());
        if (_readPosition == _size) {
            _readPosition = 0U;
            _size = 0U;
        }
    }

private:
    std::array<uint8_t, MAX_MESSAGE_LENGTH + LogHeader::PACKED_SIZE> _formatBuffer {};
    std::size_t _size {0U};
    std::size_t _readPosition {0U};
    std::size_t _reservedSize {0U};
};

}  // namespace se_oss
