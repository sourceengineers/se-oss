/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>

namespace se_oss {
class IWriter
{
protected:
    IWriter() = default;

public:
    virtual ~IWriter() = default;
    IWriter(const IWriter&) = default;
    IWriter(IWriter&&) = default;
    IWriter& operator=(const IWriter&) = delete;
    IWriter& operator=(IWriter&&) = delete;

    /**
     * Writes data to the underlying output.
     * @param data Pointer to the data to write.
     * @param length Number of bytes to write.
     */
    virtual void write(const void* data, std::size_t length) = 0;

    /**
     * Flushes any buffered data to the output.
     */
    virtual void flush() = 0;
};
}  // namespace se_oss
