/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/buffer/ImmediateBuffer.h"
#include "se-oss/log/format/CborFormatter.h"
#include "se-oss/log/format/PrintfFormatter.h"


#if defined FORMAT_CUSTOM

namespace se_oss {
namespace log_conf {

constexpr std::size_t BUFFER_SIZE {2048U};
constexpr std::size_t MAX_MESSAGE_LENGTH {128U};

class MyFormatter
{
public:
    template<typename... Values>
    static size_t format(
        void* buffer,
        std::size_t bufferSize,
        const se_oss::LogRecord& record,
        const char* const formatString,
        const Values&... values
    )
    {
        StringBuffer string {buffer, bufferSize};
        string.appendTime("%y%m%dT%H%M%S", record.timestamp);
        string.append("| %s | ", toString(record.metadata.level));
        string.append("%s | ", record.loggerName);
        string.append(formatString, std::forward<const Values>(values)...);
        string.append("\n");
        return string.length();
    }
};

// #define SE_OSS_LOG_REPLACE_STRINGS

using Formatter = MyFormatter;

using Buffer = ImmediateBuffer<MAX_MESSAGE_LENGTH>;

constexpr LogLevel MAX_LOG_LEVEL {LogLevel::TRACE};
} // namespace log_conf
} // namespace se_oss

#else

#include "default_conf/UserLogConf.h"

#endif
