/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogFilter.h"
#include "LogFilter.h"
#include "buffer/IBuffer.h"
#include "sink/ILogSink.h"

#include <atomic>
#include <memory>

#include "UserLogConf.h"

namespace se_oss {

/**
 * Structure holding statistics about the logger's operation.
 */
struct LogStatistics
{
    uint32_t droppedMessages {0}; /**< Number of messages dropped due to buffer overflow or other issues. */
};

class LogContext : public ILogFilter
{
public:
    LogContext(uint8_t tag, const char* name, ILogSink& sink, TimeProvider timeProvider) :
        _contextTag {tag},
        _name {name},
        _sink {sink},
        _timeProvider {std::move(timeProvider)}
    {
    }
    ~LogContext() override = default;
    LogContext(const LogContext&) = delete;
    LogContext(LogContext&&) = delete;
    LogContext& operator=(const LogContext&) = delete;
    LogContext& operator=(LogContext&&) = delete;

    uint8_t contextTag() const { return _contextTag; }
    void setContextTag(uint8_t tag) { _contextTag = tag; }
    const char* name() const { return _name; }
    LogStatistics statistics() const { return _statistics; }
    uint64_t time() const { return _timeProvider ? _timeProvider() : INVALID_TIME; }

    bool passesFilter(LogMetadata metadata) const { return _filter.passesFilter(metadata); }
    void writeMessage(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer);

    /**
     * Distributes messages from the buffer to the sink.
     *
     * Note: This method does nothing because in immediate mode, the message was already sent.
     *
     * @param maxNumberOfMessages Maximum number of messages to process in this call.
     */
    template<class TBuffer = log_conf::Buffer>
    std::enable_if_t<log_detail::is_immediate_buffer<TBuffer>::value, void> distributeMessages(std::size_t maxNumberOfMessages = 20U)
    {
        (void)maxNumberOfMessages;
        return;
    }

    /**
     * Distributes messages from the buffer to the sink.
     *
     * This is only relevant for deferred logging (using AtomicBuffer).
     * For immediate logging (ImmediateBuffer), this method does nothing.
     *
     * @param maxNumberOfMessages Maximum number of messages to process in this call.
     */
    template<class TBuffer = log_conf::Buffer>
    std::enable_if_t<!log_detail::is_immediate_buffer<TBuffer>::value, void> distributeMessages(std::size_t maxNumberOfMessages = 20U)
    {
        bool readSuccessful {true};
        for (size_t i = 0; i < maxNumberOfMessages && readSuccessful; ++i) {
            readSuccessful = distributeSingleMessage();
        }
    }

    // ILogFilter realization
    void setLogLevel(LogLevel level) override { return _filter.setLogLevel(level); }
    void setFilter(LogFilterFunction filter) override { return _filter.setFilter(filter); }

private:
    LogFilter _filter {};
    uint8_t _contextTag {0U};
    const char* _name {nullptr};
    log_conf::Buffer _buffer {};
    ILogSink& _sink;
    LogStatistics _statistics {};
    const TimeProvider _timeProvider {};

    bool distributeSingleMessage();
};

}  // namespace se_oss
