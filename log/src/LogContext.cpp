/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/LogContext.h"

namespace se_oss {

IBuffer::WriteRegion LogContext::reserveMessage(std::size_t size)
{
    auto region = _buffer.reserveWrite(size);
    if (region.data == nullptr || region.size == 0U) {
        _statistics.droppedMessages++;
    }
    return region;
}

void LogContext::commitMessage(std::size_t bytesWritten)
{
    _buffer.commitWrite(bytesWritten);

    // In immediate mode the message is handed to the sink right away, so that step has to succeed
    // too. A deferred buffer is drained later by distributeMessages(); a successful write is all
    // there is to check here.
    if (log_detail::is_immediate_buffer<log_conf::Buffer>::value && !distributeSingleMessage()) {
        _statistics.droppedMessages++;
    }
}

bool LogContext::distributeSingleMessage()
{
    auto region = _buffer.acquireRead();
    if (region.data == nullptr) {
        return false;
    }

    LogHeader header {};
    auto* message = deserialize(header, region.data, region.size);
    _sink.write(header.metadata, message, header.messageLength);
    _buffer.consumeRead(LogHeader::PACKED_SIZE + header.messageLength);
    return true;
}

}  // namespace se_oss
