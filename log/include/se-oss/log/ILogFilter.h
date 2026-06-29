/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Types.h"

#include <functional>

namespace se_oss {

using LogFilterFunction = std::function<bool(const LogMetadata&)>;

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

    /**
     * Sets the minimum log level for the filter.
     * @param level The minimum log level.
     */
    virtual void setLogLevel(LogLevel level) = 0;

    /**
     * Sets a custom filter function.
     * @param filter A function that takes LogMetadata and returns true to allow the message.
     */
    virtual void setFilter(LogFilterFunction filter) = 0;
};
}  // namespace se_oss
