/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IBuffer.h"
#include "se-oss/log/sink/ILogSink.h"

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
class NoBuffer final : public IBuffer
{
public:
    NoBuffer() = default;
    ~NoBuffer() override = default;
    NoBuffer(const NoBuffer&) = delete;
    NoBuffer(NoBuffer&&) = delete;
    NoBuffer& operator=(const NoBuffer&) = delete;
    NoBuffer& operator=(NoBuffer&&) = delete;

    std::size_t capacity() const override { return _formatBuffer.size(); }
    std::size_t size() const override { return 0U; }
    std::size_t free() const override { return _formatBuffer.size(); }

    bool read(const std::function<std::size_t(const void*, std::size_t)>& consumer) override
    {
        return consumer(_formatBuffer.data(), _size) > 0U;
    }

    bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer) override
    {
        if (reserveSize > _formatBuffer.size()) {
            return false;
        }
        _size = producer(_formatBuffer.data(), _formatBuffer.size());
        return _size > 0U;
    }

private:
    std::array<uint8_t, MAX_MESSAGE_LENGTH> _formatBuffer {};
    std::size_t _size {0U};
};

}  // namespace se_oss
