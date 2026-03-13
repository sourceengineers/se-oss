/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <atomic>

namespace se_oss {

/**
 * Structure holding statistics about the logger's operation.
 */
struct LogStatistics
{
    uint32_t droppedMessages {0}; /**< Number of messages dropped due to buffer overflow or other issues. */
};


class LogContext
{
public:
    LogContext(uint8_t tag, const char* name, ILogSink& sink, TimeProvider  timeProvider) :
        _contextTag {tag},
        _name {name},
        _sink {sink},
        _timeProvider {std::move(timeProvider)}
    { }
    ~LogContext() = default;
    LogContext(const LogContext&) = delete;
    LogContext(LogContext&&) = delete;
    LogContext& operator=(const LogContext&) = delete;
    LogContext& operator=(LogContext&&) = delete;

    uint8_t contextTag() const { return _contextTag; }
    const char* name() const { return _name; }
    LogStatistics statistics() const { return _statistics; }
    uint64_t time() const
    {
        return _timeProvider ? _timeProvider() : UINT64_MAX;
    }

    void writeMessage(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer)
    {
        bool writeSuccessful = _buffer->write(reserveSize, producer);

        // todo
        // if (writeSuccessful && log_detail::isImmediate()) {
        //     _buffer->read([&](const void* buffer, std::size_t size) {
        //         _sink.write(record.metadata, buffer, size);
        //         return size;
        //     });
        // }

        if (!writeSuccessful) {
            _statistics.droppedMessages++;
        }
    }

    /**
     * Distributes messages from the buffer to the sink.
     *
     * This is only relevant for deferred logging (using AtomicBuffer).
     * For immediate logging (NoBuffer), this method does nothing.
     *
     * @param maxNumberOfMessages Maximum number of messages to process in this call.
     */
    void distributeMessages(std::size_t maxNumberOfMessages = 20U) const;

private:
    // todo: add filter class
    static constexpr LogLevel MAX_LEVEL {toLogLevel(SE_LOG_MAX_LOG_LEVEL)};
    std::atomic<LogLevel> _level {LogLevel::INFO};

    uint8_t _contextTag {0U};
    const char* _name {nullptr};
    std::unique_ptr<IBuffer> _buffer {log_detail::createBuffer()};
    ILogSink& _sink;
    LogStatistics _statistics {};
    const TimeProvider _timeProvider {};
};

inline void LogContext::distributeMessages(std::size_t maxNumberOfMessages) const
{
    if (log_detail::isImmediate()) {
        return;
    }

    bool readSuccessful {true};
    for (size_t i = 0; i < maxNumberOfMessages && readSuccessful; ++i) {
        readSuccessful = _buffer->read([&](const void* buffer, std::size_t size) {
            LogHeader header {};
            auto* bufferPosition = deserialize(header, buffer, size);
            _sink.write(header.metadata, bufferPosition, header.messageLength);
            return LogHeader::PACKED_SIZE + header.messageLength;
        });
    }
}
} // namespace se_oss
