/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Conf.h"
#include "buffer/AtomicBuffer.h"
#include "sink/ILogSink.h"

#ifdef SE_OSS_LOG_REPLACE_STRINGS
#include "ResourceDatabase.h"
#endif

#include <memory>

#ifndef SE_OSS_LOG_REPLACE_STRINGS

/**
 * @def LOG_TRACE(logger, format, ...)
 *
 * Logs a message with TRACE level.
 * @param logger The Logger instance.
 * @param ... The format string and additional arguments.
 */
#define LOG_TRACE(logger, ...)                              \
    do {                                                    \
        logger.log(se_oss::LogLevel::TRACE, ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_DEBUG(logger, format, ...)
 *
 * Logs a message with DEBUG level.
 * @param logger The Logger instance.
 * @param ... The format string and additional arguments
 */
#define LOG_DEBUG(logger, ...)                              \
    do {                                                    \
        logger.log(se_oss::LogLevel::DEBUG, ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_INFO(logger, format, ...)
 *
 * Logs a message with INFO level.
 * @param logger The Logger instance.
 * @param ... The format string and additional arguments
 */
#define LOG_INFO(logger, ...)                              \
    do {                                                   \
        logger.log(se_oss::LogLevel::INFO, ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_WARN(logger, format, ...)
 *
 * Logs a message with WARN level.
 * @param logger The Logger instance.
 * @param ... The format string and additional arguments
 */
#define LOG_WARN(logger, ...)                              \
    do {                                                   \
        logger.log(se_oss::LogLevel::WARN, ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_ERROR(logger, format, ...)
 *
 * Logs a message with ERROR level.
 * @param logger The Logger instance.
 * @param ... The format string and additional arguments
 */
#define LOG_ERROR(logger, ...)                              \
    do {                                                    \
        logger.log(se_oss::LogLevel::ERROR, ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_FATAL(logger, format, ...)
 *
 * Logs a message with FATAL level.
 * @param logger The Logger instance.
 * @param ... The format string and additional arguments
 */
#define LOG_FATAL(logger, ...)                              \
    do {                                                    \
        logger.log(se_oss::LogLevel::FATAL, ##__VA_ARGS__); \
    } while (false)

#else

#define LOG_INTERNAL_STRING_TYPE(format) \
    struct FormatString                  \
    {                                    \
        const char* characters = format; \
    }
#define LOG_INTERNAL_RESOURCE_ID(format)                                                                       \
    se_oss::getResourceId<                                                                                     \
        decltype(se_oss::buildResourceIdentifier<FormatString>(std::make_index_sequence<sizeof(format)>()))>()

/**
 * @def LOG_TRACE(logger, format, ...)
 *
 * Logs a message with TRACE level.
 * @param logger The Logger instance.
 * @param format The format string.
 * @param ... Additional arguments.
 */
#define LOG_TRACE(logger, format, ...)                                                        \
    do {                                                                                      \
        LOG_INTERNAL_STRING_TYPE(format);                                                     \
        logger.log(se_oss::LogLevel::TRACE, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_DEBUG(logger, format, ...)
 *
 * Logs a message with DEBUG level.
 * @param logger The Logger instance.
 * @param format The format string.
 * @param ... Additional arguments.
 */
#define LOG_DEBUG(logger, format, ...)                                                        \
    do {                                                                                      \
        LOG_INTERNAL_STRING_TYPE(format);                                                     \
        logger.log(se_oss::LogLevel::DEBUG, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_INFO(logger, format, ...)
 *
 * Logs a message with INFO level.
 * @param logger The Logger instance.
 * @param format The format string.
 * @param ... Additional arguments.
 */
#define LOG_INFO(logger, format, ...)                                                        \
    do {                                                                                     \
        LOG_INTERNAL_STRING_TYPE(format);                                                    \
        logger.log(se_oss::LogLevel::INFO, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_WARN(logger, format, ...)
 *
 * Logs a message with WARN level.
 * @param logger The Logger instance.
 * @param format The format string.
 * @param ... Additional arguments.
 */
#define LOG_WARN(logger, format, ...)                                                        \
    do {                                                                                     \
        LOG_INTERNAL_STRING_TYPE(format);                                                    \
        logger.log(se_oss::LogLevel::WARN, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_ERROR(logger, format, ...)
 *
 * Logs a message with ERROR level.
 * @param logger The Logger instance.
 * @param format The format string.
 * @param ... Additional arguments.
 */
#define LOG_ERROR(logger, format, ...)                                                        \
    do {                                                                                      \
        LOG_INTERNAL_STRING_TYPE(format);                                                     \
        logger.log(se_oss::LogLevel::ERROR, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); \
    } while (false)

/**
 * @def LOG_FATAL(logger, format, ...)
 *
 * Logs a message with FATAL level.
 * @param logger The Logger instance.
 * @param format The format string.
 * @param ... Additional arguments.
 */
#define LOG_FATAL(logger, format, ...)                                                        \
    do {                                                                                      \
        LOG_INTERNAL_STRING_TYPE(format);                                                     \
        logger.log(se_oss::LogLevel::FATAL, LOG_INTERNAL_RESOURCE_ID(format), ##__VA_ARGS__); \
    } while (false)

#endif

namespace se_oss {

/**
 * Structure holding statistics about the logger's operation.
 */
struct LogStatistics
{
    uint32_t droppedMessages {0}; /**< Number of messages dropped due to buffer overflow or other issues. */
};

/**
 * The main Logger class.
 *
 * This class is responsible for logging messages to a configured sink via a buffer.
 * It manages the log level, statistics, and message distribution.
 */
class Logger final
{
public:
    /**
     * Constructs a new Logger.
     *
     * @param id A unique identifier for this logger source (0-255).
     * @param name A human-readable name for this logger.
     * @param sink The sink to write log messages to.
     * @param timeProvider Optional function to provide a timestamp (microseconds).
     */
    Logger(uint8_t id, const char* name, ILogSink& sink, const std::function<uint64_t()>& timeProvider = nullptr);
    ~Logger() = default;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    /**
     * Logs a message with the specified level and format.
     *
     * @tparam TFormat The type of the format string (const char* or resource ID).
     * @tparam Values The types of the arguments.
     * @param level The log level of the message.
     * @param format The format string.
     * @param values The arguments to format.
     */
    template<typename TFormat, typename... Values>
    void log(LogLevel level, TFormat format, const Values&... values);

    /**
     * Sets the minimum log level.
     * Messages with a level lower than this will be ignored.
     * @param level The new minimum log level.
     */
    void setLogLevel(LogLevel level) { _level = level; }

    /**
     * Retrieves current logger statistics.
     * @return A copy of the LogStatistics structure.
     */
    LogStatistics statistics() const { return _statistics; }

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
    static constexpr LogLevel MAX_LEVEL {toLogLevel(SE_LOG_MAX_LOG_LEVEL)};
    std::atomic<LogLevel> _level {LogLevel::INFO};
    std::unique_ptr<IBuffer> _buffer {log_detail::createBuffer()};
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

template<typename TFormat, typename... Values>
void Logger::log(LogLevel level, TFormat format, const Values&... values)
{
    static_assert(
        std::is_same<TFormat, const char*>::value || std::is_same<TFormat, uint32_t>::value,
        "Format type must be either const char* or uint32_t"
    );

    if (level < MAX_LEVEL || level < _level) {
        return;
    }

    LogRecord record = createRecord(level);
    std::size_t maxMessageLength = log_detail::maxMessageLength();

    bool writeSuccessful {false};
    if (log_detail::isImmediate()) {
        // immediate logging
        writeSuccessful = _buffer->write(maxMessageLength, [&](void* buffer, std::size_t size) {
            auto* byteBuffer = static_cast<uint8_t*>(buffer);
            return log_detail::format(byteBuffer, size, record, format, std::forward<const Values>(values)...);
        });

        if (writeSuccessful) {
            _buffer->read([&](const void* buffer, std::size_t size) {
                _sink.write(record.metadata, buffer, size);
                return size;
            });
        } else {
            _statistics.droppedMessages++;
        }
    } else {
        // deferred logging
        writeSuccessful =
            _buffer->write(maxMessageLength + LogHeader::PACKED_SIZE, [&](void* buffer, std::size_t size) {
                uint8_t* byteBuffer = static_cast<uint8_t*>(buffer) + LogHeader::PACKED_SIZE;
                std::size_t usableBufferSize = size - LogHeader::PACKED_SIZE;
                std::size_t bytesWritten = log_detail::format(
                    byteBuffer,
                    usableBufferSize,
                    record,
                    format,
                    std::forward<const Values>(values)...
                );
                if (bytesWritten > 0U) {
                    LogHeader header {};
                    header.metadata = record.metadata;
                    header.messageLength = bytesWritten;
                    (void)serialize(header, buffer, LogHeader::PACKED_SIZE);
                    bytesWritten += LogHeader::PACKED_SIZE;
                }
                return bytesWritten;
            });
    }

    if (!writeSuccessful) {
        _statistics.droppedMessages++;
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

}  // namespace se_oss
