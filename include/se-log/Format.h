/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_FORMAT_FORMAT_H
#define SE_SE_LOG_FORMAT_FORMAT_H

#include "se-log/Types.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <utility>

namespace se_oss {

class NullFormatter
{
public:
    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString, const Values& ...values)
    {
        (void) this;
        VoidSink {buffer, bufferSize, record, formatString, values...};
        return 0U;
    }

    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString)
    {
        (void) this;
        VoidSink {buffer, bufferSize, record, formatString};
        return 0U;
    }

private:
    struct VoidSink { template<typename... Args> explicit VoidSink(const Args& ...) {} };
};

template <typename...>
auto logFormatter() { return NullFormatter{}; }

template <typename... DummyArgs, typename... Values>
auto format(void* buffer, std::size_t bufferSize, LogRecord record, const char *const formatString, const Values& ...values) -> size_t {
    return logFormatter<DummyArgs...>().format(buffer, bufferSize, record, formatString, std::forward<const Values>(values)...);
}

template <typename... DummyArgs, typename... Values>
auto format(void* buffer, std::size_t bufferSize, LogRecord record, uint32_t formatStringId, const Values& ...values) -> size_t {
    return logFormatter<DummyArgs...>().format(buffer, bufferSize, record, formatStringId, std::forward<const Values>(values)...);
}

} // namespace se

#endif // SE_SE_LOG_FORMAT_FORMAT_H