/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstddef>

namespace se_oss {
/**
 * Interface for log buffers.
 *
 * Defines the contract for buffers used to store log messages before they are processed by a sink.
 */
class IBuffer
{
protected:
    IBuffer() = default;

public:
    struct WriteRegion
    {
        void* data {nullptr};
        std::size_t size {0U};
    };

    struct ReadRegion
    {
        const void* data {nullptr};
        std::size_t size {0U};
    };

    virtual ~IBuffer() = default;
    IBuffer(const IBuffer&) = delete;
    IBuffer(IBuffer&&) = delete;
    IBuffer& operator=(const IBuffer&) = delete;
    IBuffer& operator=(IBuffer&&) = delete;

    /**
     * Returns the total capacity of the buffer in bytes.
     * @return The capacity.
     */
    virtual std::size_t capacity() const = 0;

    /**
     * Returns the currently used size of the buffer in bytes.
     * @return The number of bytes currently stored.
     */
    virtual std::size_t size() const = 0;

    /**
     * Returns the available free space in the buffer in bytes.
     * @return The number of bytes available for writing.
     */
    virtual std::size_t free() const = 0;

    /**
     * Reserves a contiguous region for writing.
     *
     * A successful reservation must be followed by commitWrite(), even when no bytes were written.
     * Only one reservation may be outstanding. The returned region remains valid until commitWrite() is called.
     *
     * @param size Number of contiguous bytes to reserve.
     * @return The reserved region, or {nullptr, 0U} if insufficient space is available.
     */
    virtual WriteRegion reserveWrite(std::size_t size) = 0;

    /**
     * Commits data written to the region returned by reserveWrite().
     *
     * The reserved region must not be accessed after this call.
     *
     * @param bytesWritten Number of bytes written, not exceeding the reserved region size.
     */
    virtual void commitWrite(std::size_t bytesWritten) = 0;

    /**
     * Acquires the next contiguous region available for reading.
     *
     * A successful acquisition must be followed by consumeRead(), even when no bytes were read.
     * Only one acquisition may be outstanding. The returned region remains valid until consumeRead() is called.
     *
     * @return The readable region, or {nullptr, 0U} if the buffer is empty.
     */
    virtual ReadRegion acquireRead() = 0;

    /**
     * Consumes data from the region returned by acquireRead().
     *
     * The acquired region must not be accessed after this call.
     *
     * @param bytesRead Number of bytes read, not exceeding the acquired region size.
     */
    virtual void consumeRead(std::size_t bytesRead) = 0;
};

constexpr IBuffer::ReadRegion EMPTY_READ_REGION {nullptr, 0U};
constexpr IBuffer::WriteRegion EMPTY_WRITE_REGION {nullptr, 0U};

}  // namespace se_oss
