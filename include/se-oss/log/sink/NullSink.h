/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogSink.h"

namespace se_oss {

class NullSink final : public IWriter
{
public:
    NullSink() = default;
    ~NullSink() override = default;
    NullSink(const NullSink&) = delete;
    NullSink(NullSink&&) = delete;
    NullSink& operator=(const NullSink&) = delete;
    NullSink& operator=(NullSink&&) = delete;

    void write(const void*, size_t) override {}
    void flush() override {}
};

} // namespace se
