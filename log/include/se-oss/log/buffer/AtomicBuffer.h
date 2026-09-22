/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IBuffer.h"

#include <algorithm>
#include <array>
#include <atomic>

namespace se_oss {

/**
 * Thread-safe Single Producer Single Consumer (SPSC) circular buffer.
 *
 * This buffer is designed for lock-free communication between one single producer and one single background thread
 * (consumer). Note that the read and write access are non-reentrant.
 *
 * Based on the lock-free ring-buffer by ferrous systems: https://ferrous-systems.com/blog/lock-free-ring-buffer/
 *
 * @note The buffer only requires that a single read and write instruction on the memory bus is atomic.
 * There is no need for an atomic compare-and-swap chain. Hence, the buffer is thread-safe on platforms
 * which do not support `std::atomic` as long as the `size_t` memory bus access is atomic.
 *
 * @tparam SIZE The size of the buffer in bytes.
 */
template<std::size_t SIZE = 1024>
class AtomicBuffer final : public IBuffer
{
public:
    AtomicBuffer() = default;
    ~AtomicBuffer() override = default;
    AtomicBuffer(const AtomicBuffer&) = delete;
    AtomicBuffer(AtomicBuffer&&) = delete;
    AtomicBuffer& operator=(const AtomicBuffer&) = delete;
    AtomicBuffer& operator=(AtomicBuffer&&) = delete;

    std::size_t capacity() const override { return _buffer.size() - MIN_READER_WRITER_DISTANCE; }

    std::size_t size() const override
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();

        if (writer >= reader) {
            return writer - reader;
        } else {
            return (watermark - reader) + writer;
        }
    }

    std::size_t free() const override
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();

        if (writer >= reader) {
            return (watermark - writer) + reader - MIN_READER_WRITER_DISTANCE;
        } else {
            return reader - writer - MIN_READER_WRITER_DISTANCE;
        }
    }

    WriteRegion reserveWrite(std::size_t reserveSize) override
    {
        if (reserveSize == 0U) {
            return EMPTY_WRITE_REGION;
        }
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();
        _updateWatermarkAfterWrite = false;

        // writer is behind reader -> check size up to reader
        if ((writer < reader) && (reader - writer <= reserveSize)) {
            return EMPTY_WRITE_REGION;
        }
        // writer is ahead of reader -> check size up to watermark
        if (writer >= reader && watermark - writer < reserveSize) {
            // no space until watermark try wrap around
            if (reader <= reserveSize) {
                return EMPTY_WRITE_REGION;
            }

            // adjust watermark and wrap around
            _reservedWatermark = writer;
            _updateWatermarkAfterWrite = true;
            writer = 0U;
        }

        _reservedWritePosition = writer;
        _reservedWriteSize = reserveSize;
        return {_buffer.data() + writer, reserveSize};
    }

    void commitWrite(std::size_t bytesWritten) override
    {
        bytesWritten = std::min(bytesWritten, _reservedWriteSize);
        if (bytesWritten > 0U) {
            if (_updateWatermarkAfterWrite) {
                _watermark.store(_reservedWatermark);
            }
            _writer.store(_reservedWritePosition + bytesWritten);
        }
        _reservedWriteSize = 0U;
        _updateWatermarkAfterWrite = false;
    }

    ReadRegion acquireRead() override
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();
        _updateWatermarkAfterRead = false;
        std::size_t bytesAvailable {0U};

        if (reader >= watermark) {
            reader = 0U;
            _updateWatermarkAfterRead = true;
        }

        if (reader <= writer) {
            bytesAvailable = writer - reader;
        } else {
            bytesAvailable = watermark - reader;
        }

        if (bytesAvailable == 0U) {
            return EMPTY_READ_REGION;
        }

        _acquiredReadPosition = reader;
        _acquiredReadSize = bytesAvailable;
        return {_buffer.data() + reader, bytesAvailable};
    }

    void consumeRead(std::size_t bytesRead) override
    {
        _reader.store(_acquiredReadPosition + std::min(bytesRead, _acquiredReadSize));
        if (_updateWatermarkAfterRead) {
            // The watermark is only update here when the reader is ahead of the
            // writer. On the other hand, the writer can only update the
            // watermark when it's ahead of the reader. So, there is no race
            // condition here.
            _watermark.store(_buffer.size());
        }
        _acquiredReadSize = 0U;
        _updateWatermarkAfterRead = false;
    }

private:
    static constexpr std::size_t MIN_READER_WRITER_DISTANCE {1U};
    std::array<uint8_t, SIZE + MIN_READER_WRITER_DISTANCE> _buffer {};

    std::atomic<std::size_t> _reader {0U};
    std::atomic<std::size_t> _writer {0U};
    std::atomic<std::size_t> _watermark {_buffer.size()};

    std::size_t _reservedWritePosition {0U};
    std::size_t _reservedWriteSize {0U};
    std::size_t _reservedWatermark {0U};
    bool _updateWatermarkAfterWrite {false};

    std::size_t _acquiredReadPosition {0U};
    std::size_t _acquiredReadSize {0U};
    bool _updateWatermarkAfterRead {false};
};
}  // namespace se_oss
