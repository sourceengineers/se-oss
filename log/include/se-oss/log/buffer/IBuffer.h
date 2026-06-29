/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>

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
     * Reads data from the buffer.
     *
     * @param consumer A function that consumes data. It receives a pointer to the data and the size available.
     *                 It should return the number of bytes consumed.
     * @return True if the read operation was successful (data was available), false otherwise.
     */
    virtual bool read(const std::function<std::size_t(const void*, std::size_t)>& consumer) = 0;

    /**
     * Writes data to the buffer.
     *
     * @param reserveSize The amount of linear data to reserve. This limit must not be exceeded in the write operation.
     * @param producer A function that produces data. It receives a pointer to the buffer value and the size available.
     *                 It should return the number of bytes written.
     * @return True if the write operation was successful (space was available), false otherwise.
     */
    virtual bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer) = 0;
};
}  // namespace se_oss
