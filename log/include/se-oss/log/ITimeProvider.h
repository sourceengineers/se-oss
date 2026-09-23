/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

namespace se_oss {

/**
 * Interface for retrieving the current time.
 */
class ITimeProvider
{
protected:
    ITimeProvider() = default;
public:
    virtual ~ITimeProvider() = default;
    ITimeProvider(const ITimeProvider&) = default;
    ITimeProvider(ITimeProvider&&) = default;
    ITimeProvider& operator=(const ITimeProvider&) = delete;
    ITimeProvider& operator=(ITimeProvider&&) = delete;

    /**
     * Returns the current time in microseconds.
     */
    virtual uint64_t time() const = 0;
};

}  // namespace se_oss
