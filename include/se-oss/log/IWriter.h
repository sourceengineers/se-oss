/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_IWRITER_H
#define SE_SE_LOG_IWRITER_H

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

    virtual void write(const void* data, std::size_t length) = 0;
    virtual void flush() = 0;
};
} // namespace se

#endif // SE_SE_LOG_IWRITER_H