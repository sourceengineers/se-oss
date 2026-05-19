/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/LogContext.h"

namespace se_oss {

void LogContext::writeMessage(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer)
{
    bool writeSuccessful = _buffer.write(reserveSize, producer);

    if (writeSuccessful && log_detail::is_immediate_buffer<log_conf::Buffer>::value) {
        (void)distributeSingleMessage();
    }

    if (!writeSuccessful) {
        _statistics.droppedMessages++;
    }
}

bool LogContext::distributeSingleMessage()
{
    return _buffer.read([&](const void* buffer, std::size_t size) {
            LogHeader header {};
            auto* bufferPosition = deserialize(header, buffer, size);
            _sink.write(header.metadata, bufferPosition, header.messageLength);
            return LogHeader::PACKED_SIZE + header.messageLength;
        });
}

}  // namespace se_oss
