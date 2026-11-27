/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/Conf.h"
#include "se-oss/log/Types.h"
#include "se-oss/log/filter/ILogFilterSetter.h"

// [[nodiscard]] is C++17; se-oss is also compiled as C++14 (clang: -Wc++17-attribute-extensions)
#ifndef SE_OSS_NODISCARD
#if __cplusplus >= 201703L
#define SE_OSS_NODISCARD [[nodiscard]]
#else
#define SE_OSS_NODISCARD
#endif
#endif

namespace se_oss {

class LogFilter : public ILogFilterSetter
{
public:
    LogFilter() = default;
    ~LogFilter() override = default;
    LogFilter(const LogFilter&) = delete;
    LogFilter(LogFilter&&) = delete;
    LogFilter& operator=(const LogFilter&) = delete;
    LogFilter& operator=(LogFilter&&) = delete;

    SE_OSS_NODISCARD bool passesFilter(const LogMetadata& metadata) const
    {
        // OFF disables logging: a message logged at OFF never passes, whatever the filter level
        if (metadata.level == LogLevel::OFF) {
            return false;
        }
        if (metadata.level < log_conf::MAX_LOG_LEVEL) {
            return false;
        }
        if (metadata.level < _logLevel) {
            return false;
        }
        if (_customFilter == nullptr) {
            return true;
        }

        return _customFilter->passesFilter(metadata);
    }

    // ILogFilterSetter realization
    void setLogFilterLevel(LogLevel level) override { _logLevel = level; }

    void setCustomLogFilter(const ILogFilter* filter) override { _customFilter = filter; }

private:
    LogLevel _logLevel {LogLevel::TRACE};
    const ILogFilter* _customFilter {nullptr};
};

}  // namespace se_oss
