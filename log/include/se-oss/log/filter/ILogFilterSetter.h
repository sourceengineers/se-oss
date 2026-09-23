/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogFilter.h"
#include "se-oss/log/Types.h"

namespace se_oss {

class ILogFilterSetter
{
protected:
    ILogFilterSetter() = default;

public:
    virtual ~ILogFilterSetter() = default;
    ILogFilterSetter(const ILogFilterSetter&) = default;
    ILogFilterSetter(ILogFilterSetter&&) = default;
    ILogFilterSetter& operator=(const ILogFilterSetter&) = delete;
    ILogFilterSetter& operator=(ILogFilterSetter&&) = delete;

    /**
     * Sets the minimum log level for the filter.
     * @param level The minimum log level.
     */
    virtual void setLogFilterLevel(LogLevel level) = 0;

    /**
     * Sets a custom filter object.
     * @param filter A objets that determines whether to allow a message.
     */
    virtual void setCustomLogFilter(const ILogFilter* filter) = 0;
};
}  // namespace se_oss
