/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IWriter.h"

namespace se_oss {

/**
 * Sink that writes log messages to the console (stdout).
 *
 * Can optionally output in hexadecimal format for binary log data.
 */
class ConsoleWriter final : public IWriter
{
public:
    /**
     * Constructs a ConsoleSink.
     * @param hexOutput If true, outputs data in hexadecimal string format.
     */
    explicit ConsoleWriter(bool hexOutput = false);
    ~ConsoleWriter() override = default;
    ConsoleWriter(const ConsoleWriter&) = delete;
    ConsoleWriter(ConsoleWriter&&) = delete;
    ConsoleWriter& operator=(const ConsoleWriter&) = delete;
    ConsoleWriter& operator=(ConsoleWriter&&) = delete;

    void write(const void* data, size_t length) override;
    void flush() override { }

private:
    bool _hexOutput;
};

}  // namespace se_oss
