/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/Types.h"

#include <cinttypes>
#include <cstdio>
#include <cstring>

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

    template<typename... Values>
    size_t format(void* buffer, std::size_t bufferSize, LogRecord record, uint32_t formatStringId, const Values& ...values)
    {
        (void) this;
        VoidSink {buffer, bufferSize, record, formatStringId, values...};
        return 0U;
    }

private:
    struct VoidSink { template<typename... Args> explicit VoidSink(const Args& ...) {} };
};

} // namespace se
