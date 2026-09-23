/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "../Types.h"

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

    virtual bool passesFilter(const LogMetadata& metadata) const = 0;
};
}  // namespace se_oss
