/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Conf.h"
#include "Types.h"

namespace se_oss {

class LogFilter : public ILogFilter
{
public:
    LogFilter() = default;
    ~LogFilter() override = default;
    LogFilter(const LogFilter&) = delete;
    LogFilter(LogFilter&&) = delete;
    LogFilter& operator=(const LogFilter&) = delete;
    LogFilter& operator=(LogFilter&&) = delete;

    [[nodiscard]] bool passesFilter(LogMetadata metadata) const
    {
        if (metadata.level < log_conf::MAX_LOG_LEVEL) {
            return false;
        }
        if (_filter == nullptr) {
            return false;
        }

        return _filter(metadata);
    }

    void setLogLevel(LogLevel level) override
    {
        _filter = [level](LogMetadata metadata) { return metadata.level >= level; };
    }

    void setFilter(LogFilterFunction filter) override { _filter = filter; }


private:
    LogFilterFunction _filter {[](const auto&) -> bool { return true; }};
};

} // namespace se_oss
