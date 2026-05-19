/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/LogContext.h"

namespace se_oss {

void LogContext::writeMessage(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer)
{
    bool writeSuccessful = _buffer->write(reserveSize, producer);

    if (writeSuccessful && log_detail::isImmediate()) {
        (void)distributeSingleMessage();
    }

    if (!writeSuccessful) {
        _statistics.droppedMessages++;
    }
}

void LogContext::distributeMessages(std::size_t maxNumberOfMessages) const
{
    if (log_detail::isImmediate()) {
        return;
    }

    bool readSuccessful {true};
    for (size_t i = 0; i < maxNumberOfMessages && readSuccessful; ++i) {
        readSuccessful = distributeSingleMessage();
    }
}

bool LogContext::distributeSingleMessage() const
{
    return _buffer->read([&](const void* buffer, std::size_t size) {
        LogHeader header {};
        auto* bufferPosition = deserialize(header, buffer, size);
        _sink.write(header.metadata, bufferPosition, header.messageLength);
        return LogHeader::PACKED_SIZE + header.messageLength;
    });
}

}  // namespace se_oss
