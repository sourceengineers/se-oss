/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_BUFFER_IREADER_H
#define SE_SE_LOG_BUFFER_IREADER_H
#include <functional>

namespace se_oss {
class IReader
{
protected:
    IReader() = default;
public:
    virtual ~IReader() = default;
    IReader(const IReader&) = delete;
    IReader(IReader&&) = delete;
    IReader& operator=(const IReader&) = delete;
    IReader& operator=(IReader&&) = delete;

    virtual bool read(const std::function<std::size_t(const void*, std::size_t)>& consumer) = 0;
};
} // namespace se

#endif // SE_SE_LOG_BUFFER_IREADER_H