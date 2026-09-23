/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/ITimeProvider.h"

#include <cstdint>

namespace se_oss {

/**
 * Time provider that returns a fixed, settable time, for building a LogContext in tests.
 *
 * A LogContext keeps a reference to its time provider, so the provider must outlive the
 * context: declare it before the context in a fixture, or before it in a test body.
 */
class FixedTimeProvider final : public ITimeProvider
{
public:
    /**
     * @param time Time in microseconds returned by time() until set() is called.
     */
    explicit FixedTimeProvider(uint64_t time = 0U) : _time {time} { }
    ~FixedTimeProvider() override = default;
    FixedTimeProvider(const FixedTimeProvider&) = delete;
    FixedTimeProvider(FixedTimeProvider&&) = delete;
    FixedTimeProvider& operator=(const FixedTimeProvider&) = delete;
    FixedTimeProvider& operator=(FixedTimeProvider&&) = delete;

    uint64_t time() const override { return _time; }

    /**
     * Changes the time returned from now on.
     */
    void set(uint64_t time) { _time = time; }

private:
    uint64_t _time;
};

}  // namespace se_oss
