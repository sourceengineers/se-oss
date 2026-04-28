/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Types.h"

#include <atomic>

namespace se_oss {

using FilterFunction = std::function<bool(const LogMetadata&)>;

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
        if (metadata.level < MAX_LEVEL) {
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

    void setFilter(std::function<bool(const LogMetadata&)> filter) override { _filter = filter; }


private:
    static constexpr LogLevel MAX_LEVEL {toLogLevel(SE_LOG_MAX_LOG_LEVEL)};
    FilterFunction _filter {[](const auto&) -> bool { return true; }};
};

} // namespace se_oss