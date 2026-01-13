/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Conf.h"
#include "ResourceDatabase.h"
#include "buffer/AtomicBuffer.h"
#include "sink/ILogSink.h"

#include <memory>


#ifndef SE_LOG_REPLACE_STRINGS

#define LOG_TRACE(logger, format, ...) do { logger.log(se_oss::LogLevel::TRACE, format, ##__VA_ARGS__); } while (false)
#define LOG_DEBUG(logger, format, ...) do { logger.log(se_oss::LogLevel::DEBUG, format, ##__VA_ARGS__); } while (false)
#define LOG_INFO(logger, format, ...) do { logger.log(se_oss::LogLevel::INFO, format, ##__VA_ARGS__); } while (false)
#define LOG_WARN(logger, format, ...) do { logger.log(se_oss::LogLevel::WARN, format, ##__VA_ARGS__); } while (false)
#define LOG_ERROR(logger, format, ...) do { logger.log(se_oss::LogLevel::ERROR, format, ##__VA_ARGS__); } while (false)
#define LOG_FATAL(logger, format, ...) do { logger.log(se_oss::LogLevel::FATAL, format, ##__VA_ARGS__); } while (false)

#else

#define LOG_INTERNAL_STRING_TYPE(format) struct FormatString { const char* characters = format; }
#define LOG_INTERNAL_RESOURCE_ID(format) se_oss::getResourceId<decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>()

#define LOG_TRACE(logger, format, ...) do { LOG_INTERNAL_STRING_TYPE(format); logger.log(se_oss::LogLevel::TRACE, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); } while (false)
#define LOG_DEBUG(logger, format, ...) do { LOG_INTERNAL_STRING_TYPE(format); logger.log(se_oss::LogLevel::DEBUG, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); } while (false)
#define LOG_INFO(logger, format, ...) do { LOG_INTERNAL_STRING_TYPE(format); logger.log(se_oss::LogLevel::INFO, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); } while (false)
#define LOG_WARN(logger, format, ...) do { LOG_INTERNAL_STRING_TYPE(format); logger.log(se_oss::LogLevel::WARN, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); } while (false)
#define LOG_ERROR(logger, format, ...) do { LOG_INTERNAL_STRING_TYPE(format); logger.log(se_oss::LogLevel::ERROR, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); } while (false)
#define LOG_FATAL(logger, format, ...) do { LOG_INTERNAL_STRING_TYPE(format); logger.log(se_oss::LogLevel::FATAL, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); } while (false)

#endif

namespace se_oss {

struct LogStatistics
{
    uint32_t droppedMessages {0};
};

class Logger final
{
public:
    Logger(uint8_t id, const char* name, ILogSink& sink, const std::function<uint64_t()>& timeProvider = nullptr);
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;


    template<typename TFormat, typename... Values>
    void log(LogLevel level, TFormat format, const Values&... values);

    void setLogLevel(LogLevel level) { _level = level; }
    LogStatistics statistics() const { return _statistics; }
    void distributeMessages(std::size_t maxNumberOfMessages = 20U) const;

private:
    static constexpr LogLevel MAX_LEVEL {toLogLevel(SE_LOG_MAX_LOG_LEVEL)};
    LogLevel _level {LogLevel::INFO};
    std::unique_ptr<IBuffer> _buffer {logConfCreateBuffer()};
    LogStatistics _statistics {};
    ILogSink& _sink;

    uint8_t _id;
    const char* _name {nullptr};
    const std::function<uint64_t()> _timeProvider {};

    LogRecord createRecord(LogLevel level) const;
};

inline Logger::Logger(uint8_t id, const char* name, ILogSink& sink, const std::function<uint64_t()>& timeProvider) :
    _sink {sink},
    _id {id},
    _name {name},
    _timeProvider {timeProvider}
{

}

inline void Logger::distributeMessages(std::size_t maxNumberOfMessages) const
{
    for (size_t i = 0; i < maxNumberOfMessages; ++i) {
        bool readSuccessful = _buffer->read([&](const void* buffer, std::size_t size) {
            LogHeader header {};
            auto* bufferPosition = deserialize(header, buffer, size);
            _sink.write(header.metadata, bufferPosition, header.messageLength);
            // todo: checks i guess
            return LogHeader::PACKED_SIZE + header.messageLength;
        });

        if (!readSuccessful) {
            break;
        }
    }
}

template<typename TFormat, typename... Values>
void Logger::log(LogLevel level, TFormat format, const Values&... values)
{
    static_assert(std::is_same<TFormat, const char*>::value || std::is_same<TFormat, uint32_t>::value, "Format type must be either const char* or uint32_t");

    if (level < MAX_LEVEL || level < _level) {
        return;
    }

    LogRecord record = createRecord(level);
    std::size_t maxMessageLength = logConfMaxMessageLength();

    bool writeSuccessful = _buffer->write(maxMessageLength + LogHeader::PACKED_SIZE, [&](void* buffer, std::size_t size) {
        uint8_t* byteBuffer = static_cast<uint8_t*>(buffer) + LogHeader::PACKED_SIZE;
        std::size_t usableBufferSize = size - LogHeader::PACKED_SIZE;
        std::size_t bytesWritten = logConfFormat(byteBuffer, usableBufferSize, record, format, std::forward<const Values>(values)...);
        if (bytesWritten > 0U) {
            LogHeader header {};
            header.metadata = record.metadata;
            header.messageLength = bytesWritten;
            (void)serialize(header, buffer, LogHeader::PACKED_SIZE);
            bytesWritten += LogHeader::PACKED_SIZE;
        }
        return bytesWritten;
    });

    if (!writeSuccessful) {
        _statistics.droppedMessages++;

    } else if (_buffer->capacity() == 0U) {
        distributeMessages(1U);
    }
}

inline LogRecord Logger::createRecord(LogLevel level) const
{
    LogRecord record {};
    record.metadata.level = level;
    record.metadata.sourceId = _id;
    record.sourceName = _name;
    if (_timeProvider) {
        record.timestamp = _timeProvider();
    }
    return record;
}

} // namespace se
