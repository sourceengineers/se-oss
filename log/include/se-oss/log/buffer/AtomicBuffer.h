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
 * This buffer is designed for communication between one single producer and one single background thread (consumer).
 * It uses no mutexes, but whether its atomic operations are lock-free depends on the target platform. Read and write
 * access are non-reentrant: only one write reservation and one read acquisition may be outstanding at a time.
 * Snapshot retries are bounded; transient contention makes size() and free() return zero and acquireRead() return an
 * empty region.
 *
 * Based on the lock-free ring-buffer by ferrous systems: https://ferrous-systems.com/blog/lock-free-ring-buffer/
 *
 * @tparam SIZE The size of the buffer in bytes.
 */
template<std::size_t SIZE = 1024>
class AtomicBuffer final : public IBuffer
{
    static_assert(SIZE > 0U, "AtomicBuffer capacity must be greater than zero");

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
        Positions positions {};
        if (!loadPositions(positions)) {
            return 0U;
        }

        if (positions.writer >= positions.reader) {
            return positions.writer - positions.reader;
        } else {
            return (positions.watermark - positions.reader) + positions.writer;
        }
    }

    std::size_t free() const override
    {
        Positions positions {};
        if (!loadPositions(positions)) {
            return 0U;
        }

        if (positions.writer >= positions.reader) {
            return (positions.watermark - positions.writer) + positions.reader - MIN_READER_WRITER_DISTANCE;
        } else {
            return positions.reader - positions.writer - MIN_READER_WRITER_DISTANCE;
        }
    }

    WriteRegion reserveWrite(std::size_t reserveSize) override
    {
        if (reserveSize == 0U || reserveSize > capacity() || _reservedWriteSize != 0U) {
            return EMPTY_WRITE_REGION;
        }
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();
        _updateWatermarkAfterWrite = false;

        // The consumer has wrapped to the beginning. The old watermark no longer separates two readable regions, so
        // the producer can reclaim the physical tail. Keeping watermark producer-owned avoids competing updates with
        // consumeRead().
        if (writer >= reader && watermark != _buffer.size()) {
            watermark = _buffer.size();
            _watermark.store(watermark);
        }

        // writer is behind reader -> check size up to reader
        if ((writer < reader) && (reader - writer <= reserveSize)) {
            return EMPTY_WRITE_REGION;
        }
        // writer is ahead of reader -> check size up to watermark. When the reader sits at 0, the last physical slot
        // is the guard byte between writer and reader, so the writer must not reach it.
        auto guard = (reader == 0U) ? MIN_READER_WRITER_DISTANCE : 0U;
        if (writer >= reader && watermark - writer < reserveSize + guard) {
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
                // Publish the new watermark and wrapped writer as one versioned state. Without the version, a reader
                // could combine the old writer with the new watermark and expose bytes that have not been committed.
                auto writerVersion = _writerVersion.load();
                _writerVersion.store(writerVersion + 1U);
                _watermark.store(_reservedWatermark);
                _writer.store(_reservedWritePosition + bytesWritten);
                _writerVersion.store(writerVersion + 2U);
            } else {
                _writer.store(_reservedWritePosition + bytesWritten);
            }
        }
        _reservedWriteSize = 0U;
        _updateWatermarkAfterWrite = false;
    }

    ReadRegion acquireRead() override
    {
        if (_acquiredReadSize != 0U) {
            return EMPTY_READ_REGION;
        }
        ProducerPositions producerPositions {};
        if (!loadProducerPositions(producerPositions)) {
            return EMPTY_READ_REGION;
        }
        auto writer = producerPositions.writer;
        auto reader = _reader.load();
        auto watermark = producerPositions.watermark;
        std::size_t bytesAvailable {0U};

        // The reader only wraps once the writer has wrapped as well. Without the second condition, reader == writer
        // == watermark (writer landed exactly on the physical end) would be misread as a full buffer instead of an
        // empty one, re-exposing already consumed bytes.
        if (reader >= watermark && writer < reader) {
            reader = 0U;
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
        if (_acquiredReadSize == 0U) {
            return;
        }
        _reader.store(_acquiredReadPosition + std::min(bytesRead, _acquiredReadSize));
        _acquiredReadSize = 0U;
    }

private:
    struct ProducerPositions
    {
        std::size_t writer;
        std::size_t watermark;
    };

    struct Positions
    {
        std::size_t reader;
        std::size_t writer;
        std::size_t watermark;
    };

    bool loadPositions(Positions& positions) const
    {
        for (std::size_t attempt = 0U; attempt < MAX_LOAD_ATTEMPTS; ++attempt) {
            auto firstReader = _reader.load();
            ProducerPositions producerPositions {};
            if (!loadProducerPositions(producerPositions)) {
                continue;
            }
            auto secondReader = _reader.load();
            if (firstReader != secondReader) {
                continue;
            }

            positions = {secondReader, producerPositions.writer, producerPositions.watermark};
            if (positions.writer >= positions.reader && positions.watermark != _buffer.size()) {
                positions.watermark = _buffer.size();
            }
            return true;
        }
        return false;
    }

    bool loadProducerPositions(ProducerPositions& positions) const
    {
        for (std::size_t attempt = 0U; attempt < MAX_LOAD_ATTEMPTS; ++attempt) {
            auto firstVersion = _writerVersion.load();
            if ((firstVersion & 1U) != 0U) {
                continue;
            }
            ProducerPositions candidate {_writer.load(), _watermark.load()};
            auto secondVersion = _writerVersion.load();
            if (firstVersion == secondVersion) {
                positions = candidate;
                return true;
            }
        }
        return false;
    }

    static constexpr std::size_t MAX_LOAD_ATTEMPTS {8U};
    static constexpr std::size_t MIN_READER_WRITER_DISTANCE {1U};
    std::array<uint8_t, SIZE + MIN_READER_WRITER_DISTANCE> _buffer {};

    std::atomic<std::size_t> _reader {0U};
    std::atomic<std::size_t> _writer {0U};
    std::atomic<std::size_t> _watermark {_buffer.size()};
    std::atomic<std::size_t> _writerVersion {0U};

    std::size_t _reservedWritePosition {0U};
    std::size_t _reservedWriteSize {0U};
    std::size_t _reservedWatermark {0U};
    bool _updateWatermarkAfterWrite {false};

    std::size_t _acquiredReadPosition {0U};
    std::size_t _acquiredReadSize {0U};
};
}  // namespace se_oss
