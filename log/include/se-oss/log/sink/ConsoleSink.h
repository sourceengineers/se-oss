/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogSink.h"
#include "se-oss/log/ILogFilter.h"

namespace se_oss {

/**
 * Sink that writes log messages to the console (stdout).
 *
 * Can optionally output in hexadecimal format for binary log data.
 */
class ConsoleSink final : public IWriter
{
public:
    /**
     * Constructs a ConsoleSink.
     * @param hexOutput If true, outputs data in hexadecimal string format.
     */
    explicit ConsoleSink(bool hexOutput = false);
    ~ConsoleSink() override = default;
    ConsoleSink(const ConsoleSink&) = delete;
    ConsoleSink(ConsoleSink&&) = delete;
    ConsoleSink& operator=(const ConsoleSink&) = delete;
    ConsoleSink& operator=(ConsoleSink&&) = delete;

    void write(const void* data, size_t length) override;
    void flush() override { }

private:
    bool _hexOutput;
};

}  // namespace se_oss
