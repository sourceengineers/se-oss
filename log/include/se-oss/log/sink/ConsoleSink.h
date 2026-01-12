/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogSink.h"
#include "se-oss/log/ILogFilter.h"

namespace se_oss {

class ConsoleSink final : public IWriter
{
public:
    explicit ConsoleSink(bool hexOutput = false);
    ~ConsoleSink() override = default;
    ConsoleSink(const ConsoleSink&) = delete;
    ConsoleSink(ConsoleSink&&) = delete;
    ConsoleSink& operator=(const ConsoleSink&) = delete;
    ConsoleSink& operator=(ConsoleSink&&) = delete;

    void write(const void* data, size_t length) override;
    void flush() override {}

private:
    bool _hexOutput;
    LogLevel _level {LogLevel::TRACE};
};

} // namespace se
