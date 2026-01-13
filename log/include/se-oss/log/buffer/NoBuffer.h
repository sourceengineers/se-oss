/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IBuffer.h"
#include "se-oss/log/sink/ILogSink.h"

namespace se_oss {

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
        return consumer(_formatBuffer.data(), _formatBuffer.size()) > 0;
    }

    bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer) override
    {
        if (reserveSize > _formatBuffer.size()) {
            return false;
        }
        return producer(_formatBuffer.data(), _formatBuffer.size());
    }

private:
    std::array<uint8_t, MAX_MESSAGE_LENGTH> _formatBuffer {};
};

} // namespace se
