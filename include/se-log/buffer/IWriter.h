/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_LOG_ILOGBUFFER_H
#define SE_LOG_ILOGBUFFER_H

#include "se-log/sink/ILogSink.h"

#include <functional>

namespace se_oss {


// todo where to put generic stuff such as capacity, size, empty, etc
// todo interface for Item instead of data
class IWriter
{
protected:
    IWriter() = default;
public:
    virtual ~IWriter() = default;
    IWriter(const IWriter&) = delete;
    IWriter(IWriter&&) = delete;
    IWriter& operator=(const IWriter&) = delete;
    IWriter& operator=(IWriter&&) = delete;
    
    virtual bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer) = 0;
};

} // namespace se

#endif // SE_LOG_ILOGBUFFER_H
