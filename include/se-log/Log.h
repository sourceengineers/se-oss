/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_LOG_LOG_H
#define SE_LOG_LOG_H

#include "Format.h"
#include "ILogFilter.h"
#include "buffer/AtomicCircularBufferSpSc.h"
#include "sink/ILogSink.h"
#include "ResourceDatabase.h"

#include <memory>

#ifndef SE_LOG_REPLACE_STRINGS

#define LOG_TRACE(logger, format, ...) do { logger.log(se_oss::LogLevel::TRACE, format, ##__VA_ARGS__); } while (false)
#define LOG_DEBUG(logger, format, ...) do { logger.log(se_oss::LogLevel::DEBUG, format, ##__VA_ARGS__); } while (false)
#define LOG_INFO(logger, format, ...) do { logger.log(se_oss::LogLevel::INFO, format, ##__VA_ARGS__); } while (false)
#define LOG_WARN(logger, format, ...) do { logger.log(se_oss::LogLevel::WARN, format, ##__VA_ARGS__); } while (false)
#define LOG_ERROR(logger, format, ...) do { logger.log(se_oss::LogLevel::ERROR, format, ##__VA_ARGS__); } while (false)
#define LOG_FATAL(logger, format, ...) do { logger.log(se_oss::LogLevel::FATAL, format, ##__VA_ARGS__); } while (false)

#else

#define LOG_TRACE(logger, format, ...) do { struct FormatString { const char* characters = format; }; logger.log(se_oss::LogLevel::TRACE, se_oss::getResourceId<decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>(), ##__VA_ARGS__); } while (false)
#define LOG_DEBUG(logger, format, ...) do { struct FormatString { const char* characters = format; }; logger.log(se_oss::LogLevel::DEBUG, se_oss::getResourceId<decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>(), ##__VA_ARGS__); } while (false)
#define LOG_INFO(logger, format, ...) do { struct FormatString { const char* characters = format; }; logger.log(se_oss::LogLevel::INFO, se_oss::getResourceId<decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>(), ##__VA_ARGS__); } while (false)
#define LOG_WARN(logger, format, ...) do { struct FormatString { const char* characters = format; }; logger.log(se_oss::LogLevel::WARN, se_oss::getResourceId<decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>(), ##__VA_ARGS__); } while (false)
#define LOG_ERROR(logger, format, ...) do { struct FormatString { const char* characters = format; }; logger.log(se_oss::LogLevel::ERROR, se_oss::getResourceId<decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>(), ##__VA_ARGS__); } while (false)
#define LOG_FATAL(logger, format, ...) do { struct FormatString { const char* characters = format; }; logger.log(se_oss::LogLevel::FATAL, se_oss::getResourceId<decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>(), ##__VA_ARGS__); } while (false)

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


    template<typename... Values>
    void log(LogLevel level, const char* format, const Values&... values);

    template<typename... Values>
    void log(LogLevel level, uint32_t formatId, const Values&... values);


    void setLogLevel(LogLevel level) { _level = level; }
    LogStatistics statistics() const { return _statistics; }
    void distributeMessages();

private:
    LogLevel _level {LogLevel::INFO};
    // todo: configurable buffer size
    AtomicCircularBufferSpSc<1024> _buffer {};
    LogStatistics _statistics {};
    ILogSink& _sink;

    uint8_t _id;
    const char* _name {nullptr};

    const std::function<uint64_t()> _timeProvider {};
};

inline Logger::Logger(uint8_t id, const char* name, ILogSink& sink, const std::function<uint64_t()>& timeProvider) :
    _sink {sink},
    _id {id},
    _name {name},
    _timeProvider {timeProvider}
{

}

inline void Logger::distributeMessages()
{
    // todo: make number of messages configurable
    for (size_t i = 0; i < 20U; ++i) {
        bool readSuccessful = _buffer.read([&](const void* buffer, std::size_t size) {
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

template<typename ... Values>
void Logger::log(LogLevel level, const char* format, const Values&... values)
{
    if (level < _level) {
        return;
    }

    LogRecord record {};
    record.metadata.logLevel = level;
    record.metadata.sourceId = _id;
    // todo: tag
    record.sourceName = _name;
    if (_timeProvider) {
        record.timestamp = _timeProvider();
    }

    // todo: configure format size
    bool writeSuccessful = _buffer.write(256, [&](void* buffer, size_t size) {
        std::size_t bytesWritten = ::se_oss::format(static_cast<uint8_t*>(buffer) + LogHeader::PACKED_SIZE, size - LogHeader::PACKED_SIZE, record, format, std::forward<const Values>(values)...);
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
    }
}

template<typename ... Values>
void Logger::log(LogLevel level, uint32_t formatId, const Values&... values)
{
    if (level < _level) {
        return;
    }

    LogRecord record {};
    record.metadata.logLevel = level;
    record.metadata.sourceId = _id;
    // todo: tag
    record.sourceName = _name;
    if (_timeProvider) {
        record.timestamp = _timeProvider();
    }

    // todo: configure format size
    bool writeSuccessful = _buffer.write(256, [&](void* buffer, size_t size) {
        std::size_t bytesWritten = ::se_oss::format(static_cast<uint8_t*>(buffer) + LogHeader::PACKED_SIZE, size - LogHeader::PACKED_SIZE, record, formatId, std::forward<const Values>(values)...);
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
    }
}

} // namespace se

#endif // SE_LOG_LOG_H
