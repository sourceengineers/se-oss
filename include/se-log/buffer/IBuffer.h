/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_BUFFER_IREADER_H
#define SE_SE_LOG_BUFFER_IREADER_H

#include <functional>

namespace se_oss {
class IBuffer
{
protected:
    IBuffer() = default;
public:
    virtual ~IBuffer() = default;
    IBuffer(const IBuffer&) = delete;
    IBuffer(IBuffer&&) = delete;
    IBuffer& operator=(const IBuffer&) = delete;
    IBuffer& operator=(IBuffer&&) = delete;

    virtual std::size_t capacity() const = 0;
    virtual std::size_t size() const = 0;
    virtual std::size_t free() const = 0;
    virtual bool read(const std::function<std::size_t(const void*, std::size_t)>& consumer) = 0;
    virtual bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer) = 0;
};
} // namespace se

#endif // SE_SE_LOG_BUFFER_IREADER_H