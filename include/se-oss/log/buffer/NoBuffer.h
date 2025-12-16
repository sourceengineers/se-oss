/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_BUFFER_NOBUFFER_H
#define SE_SE_LOG_BUFFER_NOBUFFER_H

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


    std::size_t capacity() const override { return 0U; }
    std::size_t size() const override { return 0U; }
    std::size_t free() const override { return 0U; }

    bool read(const std::function<std::size_t(const void*, std::size_t)>& consumer) override
    {
        if (_newData == false) {
            return false;
        }
        bool success = consumer(_formatBuffer.data(), _formatBuffer.size()) > 0;
        if (success) {
            _newData = false;
        }
        return _newData;
    }

    bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer) override
    {
        (void) reserveSize;
        _newData = producer(_formatBuffer.data(), _formatBuffer.size());
        return _newData;
    }

private:
    std::array<uint8_t, MAX_MESSAGE_LENGTH + LogHeader::PACKED_SIZE> _formatBuffer {};
    bool _newData {false};
};

} // namespace se

#endif // SE_SE_LOG_BUFFER_NOBUFFER_H