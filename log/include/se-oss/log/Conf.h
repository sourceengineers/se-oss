/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "buffer/IBuffer.h"
#include "buffer/NoBuffer.h"
#include "format/PrintfFormatter.h"

#include <memory>

namespace se_oss {

/**
 * Glocal configuration for the logging system.
 *
 * This struct defines the compile-time configuration for the logger, including
 * the formatter to use, the buffer strategy, and the maximum message length.
 *
 * @tparam Formatter The formatter class (e.g., PrintfFormatter, CborFormatter).
 * @tparam Buffer The buffer class (e.g., AtomicBuffer, NoBuffer).
 * @tparam MAX_MESSAGE_LENGTH The maximum size in bytes for a single log message.
 */
template<class Formatter, class Buffer, std::size_t MAX_MESSAGE_LENGTH>
class  LogConf
{
    template <typename T>
    struct is_immediate_buffer : std::false_type {};

    template <std::size_t N>
    struct is_immediate_buffer<NoBuffer<N>> : std::true_type {};

public:
    /**
     * Creates a new formatter instance.
     * @return A new instance of the configured Formatter.
     */
    static constexpr Formatter formatter() { return Formatter{}; }

    /**
     * Gets the maximum message length.
     * @return The maximum message length in bytes.
     */
    static constexpr std::size_t maxMessageLength() { return MAX_MESSAGE_LENGTH; }

    /**
     * Creates a new buffer instance.
     * @return A unique pointer to the configured Buffer.
     */
    static std::unique_ptr<IBuffer> createBuffer() { return std::make_unique<Buffer>(); }

    /**
     * Checks if the logging is immediate (synchronous) or deferred.
     * @return True if the buffer strategy implies immediate logging, false otherwise.
     */
    static constexpr bool isImmediate() { return is_immediate_buffer<Buffer>::value; }
};

constexpr std::size_t DEFAULT_MAX_MESSAGE_LENGTH {128};
/**
 * Default log configuration
 * - Printf style
 * - Output to console
 * - Max message length of 128 characters
 */
using DefaultLogConf = LogConf<PrintfFormatter<TimeFormat::ISO8601>, NoBuffer<DEFAULT_MAX_MESSAGE_LENGTH>, DEFAULT_MAX_MESSAGE_LENGTH>;

/**
 * Customization point for logger configuration.
 *
 * This function template can be specialized to provide custom configuration
 * for the logging system.
 *
 * Example usage:
 * @code
 * template <>
 * auto se_oss::logConf<>()
 * {
 *     return LogConf<PrintfFormatter, AtomicBuffer<2048>, 256>{};
 * }
 * @endcode
 *
 * @return The configuration object.
 */
template <typename...>
auto logConf() { return DefaultLogConf{}; }


namespace log_detail {

template<typename... DummyArgs>
bool isImmediate()
{
    return logConf<DummyArgs...>().isImmediate();
}

template<typename... DummyArgs, typename... Arguments>
std::size_t format(Arguments&&... arguments)
{
    return decltype(logConf<DummyArgs...>().formatter())::format(arguments...);
}

template<typename... DummyArgs>
std::unique_ptr<IBuffer> createBuffer()
{
    return logConf<DummyArgs...>().createBuffer();
}

template<typename... DummyArgs>
std::size_t maxMessageLength()
{
    return logConf<DummyArgs...>().maxMessageLength();
}

}

} // namespace se_oss_oss
