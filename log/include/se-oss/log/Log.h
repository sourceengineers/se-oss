/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Conf.h"
#include "LogContext.h"
#include "buffer/AtomicBuffer.h"
#include "sink/ILogSink.h"

#ifdef SE_OSS_LOG_REPLACE_STRINGS
#include "ResourceDatabase.h"
#endif

#include <memory>
#include <utility>

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
     * @param name A human-readable name for this logger.
     * @param context Thread context the log instance acts on
     */
    explicit Logger(LogContext& context);
    ~Logger() = default;
    // Note: The intention is that a logger can be copied and adjusted locally.
    //       The log context is managed by an internal reference.
    Logger(const Logger&) = default;
    Logger(Logger&&) = default;
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
    // void setLogLevel(LogLevel level) { _context._level = level; }
    // todo

    /**
     * Retrieves current logger statistics.
     * @return A copy of the LogStatistics structure.
     */
    LogStatistics statistics() const { return _context.statistics(); }

private:
    LogContext& _context;

    uint8_t _logTag {0U};
    const char* _name {nullptr};

    LogRecord createRecord(LogLevel level) const;
};

inline Logger::Logger(LogContext& context) :
    _name {context.name()},
    _context {context}
{
}

template<typename TFormat, typename... Values>
void Logger::log(LogLevel level, TFormat format, const Values&... values)
{
    static_assert(
        std::is_same<TFormat, const char*>::value || std::is_same<TFormat, uint32_t>::value,
        "Format type must be either const char* or uint32_t"
    );

    // todo: move filter log instance
    // if (level < LogContext::MAX_LEVEL || level < _context._level) {
    //     return;
    // }

    LogRecord record = createRecord(level);
    std::size_t maxMessageLength = log_detail::maxMessageLength();

    if (log_detail::isImmediate()) {
        // immediate logging
        _context.writeMessage(maxMessageLength, [&](void* buffer, std::size_t size) {
            auto* byteBuffer = static_cast<uint8_t*>(buffer);
            return log_detail::format(byteBuffer, size, record, format, std::forward<const Values>(values)...);
        });
    } else {
        // deferred logging
        _context.writeMessage(maxMessageLength + LogHeader::PACKED_SIZE, [&](void* buffer, std::size_t size) {
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
}

inline LogRecord Logger::createRecord(LogLevel level) const
{
    LogRecord record {};
    record.metadata.level = level;
    record.metadata.contextTag = _context.contextTag();
    record.loggerName = _name;
    record.timestamp = _context.time();
    return record;
}

}  // namespace se_oss
