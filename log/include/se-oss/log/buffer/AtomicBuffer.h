/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "IBuffer.h"

#include <array>
#include <atomic>
#include <functional>

namespace se_oss {
/**
 * Atomic circular SPSC buffer
 *
 * @tparam SIZE
 *
 * Based on the lock-free ring-buffer by ferrous systems: https://ferrous-systems.com/blog/lock-free-ring-buffer/
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

    [[nodiscard]] std::size_t capacity() const override { return _buffer.size() - MIN_READER_WRITER_DISTANCE; }

    [[nodiscard]] std::size_t size() const override
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

    [[nodiscard]] std::size_t free() const override
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


    bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer) override
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();
        bool updateWatermark {false};

        // writer is behind reader -> check size up to reader
        if (writer < reader && reader - writer <= reserveSize) {
            return false;
        }
        // writer is ahead of reader -> check size up to watermark
        if (writer >= reader && watermark - writer < reserveSize) {
            // no space until watermark try wrap around
            if (reader <= reserveSize) {
                return false;
            }

            // adjust watermark and wrap around
            watermark = writer;
            updateWatermark = true;
            writer = 0U;
        }

        auto bytesWritten = producer(_buffer.data() + writer, reserveSize);
        if (bytesWritten > 0U) {
            if (updateWatermark) {
                _watermark.store(watermark);
            }
            _writer.store(writer + std::min(bytesWritten, reserveSize));
        }
        return true;
    }

    bool read(const std::function<std::size_t(const void*, std::size_t)>& consumer) override
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();
        bool updateWatermark {false};
        std::size_t bytesAvailable {0U};

        if (reader >= watermark) {
            reader = 0U;
            watermark = _buffer.size();
            updateWatermark = true;
        }

        if (reader <= writer) {
            bytesAvailable = writer - reader;
        } else {
            bytesAvailable = watermark - reader;
        }

        if (bytesAvailable == 0U) {
            return false;
        }


        auto bytesRead = consumer(_buffer.data() + reader, bytesAvailable);
        _reader.store(reader + std::min(bytesRead, bytesAvailable));
        if (updateWatermark) {
            // The watermark is only update here when the reader is ahead of the
            // writer. On the other hand, the writer can only update the
            // watermark when it's ahead of the reader. So, there is no race
            // condition here.
            _watermark.store(watermark);
        }
        return true;
    }

private:
    static constexpr std::size_t MIN_READER_WRITER_DISTANCE {1U};
    std::array<uint8_t, SIZE + MIN_READER_WRITER_DISTANCE> _buffer {};

    std::atomic<std::size_t> _reader {0U};
    std::atomic<std::size_t> _writer {0U};
    std::atomic<std::size_t> _watermark {_buffer.size()};

};
} // namespace se
