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

template<class Formatter, class Buffer, std::size_t MAX_MESSAGE_LENGTH>
class  LogConf
{
    template <typename T>
    struct is_immediate_buffer : std::false_type {};

    template <std::size_t N>
    struct is_immediate_buffer<NoBuffer<N>> : std::true_type {};

public:
    static constexpr Formatter formatter() { return Formatter{}; }
    static constexpr std::size_t maxMessageLength() { return MAX_MESSAGE_LENGTH; }
    static std::unique_ptr<IBuffer> createBuffer() { return std::make_unique<Buffer>(); }

    static constexpr bool isImmediate() { return is_immediate_buffer<Buffer>::value; }
};

constexpr std::size_t DEFAULT_MAX_MESSAGE_LENGTH {128};
using DefaultLogConf = LogConf<PrintfFormatter, NoBuffer<DEFAULT_MAX_MESSAGE_LENGTH>, DEFAULT_MAX_MESSAGE_LENGTH>;

template <typename...>
auto logConf() { return DefaultLogConf{}; }


namespace log_detail {

template<typename... DummyArgs>
std::size_t isImmediate()
{
    return logConf<DummyArgs...>().isImmediate();
}

template<typename... DummyArgs, typename... Arguments>
std::size_t format(Arguments&&... arguments)
{
    return logConf<DummyArgs...>().formatter().format(arguments...);
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

} // namespace se_oss
