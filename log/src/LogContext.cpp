/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/LogContext.h"

namespace se_oss {

void LogContext::writeMessage(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer)
{
    bool successful = _buffer.write(reserveSize, producer);

    // In immediate mode the message is handed to the sink right away, so that step has to succeed
    // too. A deferred buffer is drained later by distributeMessages(); a successful write is all
    // there is to check here.
    if (successful && log_detail::is_immediate_buffer<log_conf::Buffer>::value) {
        successful = distributeSingleMessage();
    }

    if (!successful) {
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
