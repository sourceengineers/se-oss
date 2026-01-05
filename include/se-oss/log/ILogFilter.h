/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_ILOGFILTER_H
#define SE_SE_LOG_ILOGFILTER_H

#include "Types.h"

#include <functional>

namespace se_oss {
class ILogFilter
{
protected:
    ILogFilter() = default;
public:
    virtual ~ILogFilter() = default;
    ILogFilter(const ILogFilter&) = default;
    ILogFilter(ILogFilter&&) = default;
    ILogFilter& operator=(const ILogFilter&) = delete;
    ILogFilter& operator=(ILogFilter&&) = delete;

    virtual void setLogLevel(LogLevel level) = 0;
    virtual void setFilter(std::function<bool(LogMetadata)> filter) = 0;
};
} // namespace se

#endif // SE_SE_LOG_ILOGFILTER_H