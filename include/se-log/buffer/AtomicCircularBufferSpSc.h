/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_BUFFER_ATOMICRINGBUFFER_H
#define SE_SE_LOG_BUFFER_ATOMICRINGBUFFER_H

#include "IReader.h"

#include <array>
#include <atomic>
#include <functional>

namespace se_oss {
/**
 *
 * @tparam SIZE
 *
 * Based on the lock-free ring-buffer by ferrous systems: https://ferrous-systems.com/blog/lock-free-ring-buffer/
 */
template<std::size_t SIZE = 1024>
class AtomicCircularBufferSpSc final : public IReader
{
public:
    AtomicCircularBufferSpSc() = default;
    ~AtomicCircularBufferSpSc() override = default;
    AtomicCircularBufferSpSc(const AtomicCircularBufferSpSc&) = delete;
    AtomicCircularBufferSpSc(AtomicCircularBufferSpSc&&) = delete;
    AtomicCircularBufferSpSc& operator=(const AtomicCircularBufferSpSc&) = delete;
    AtomicCircularBufferSpSc& operator=(AtomicCircularBufferSpSc&&) = delete;

    constexpr std::size_t capacity() const { return _buffer.size(); }

    // todo: test and fix +1/-1 for capacity and size

    std::size_t size() const
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();

        if (writer <= watermark) {
            return writer - reader;
        } else {
            return watermark - reader + writer;
        }
    }

    std::size_t free() const { return capacity() - size(); }


    bool write(std::size_t reserveSize, const std::function<std::size_t(void*, std::size_t)>& producer)
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();
        bool updateWatermark {false};

        // writer is behind reader -> check size up to reader
        if (writer < reader && reader - writer < reserveSize) {
            return false;
        }
        // writer is ahead of reader -> check size up to watermark
        if (writer >= reader && watermark - writer < reserveSize) {
            // no space until watermark try wrap around
            if (reader < reserveSize) {
                return false;
            }

            // adjust watermark and wrap around
            watermark = writer;
            updateWatermark = true;
            writer = 0U;
        }

        auto bytesWritten = producer(_buffer.data() + writer, reserveSize);
        if (bytesWritten > 0U) {
            // todo: sanity check
            if (updateWatermark) {
                _watermark.store(watermark);
            }
            _writer.store(writer + bytesWritten);
        }
        return true;
    }

    bool read(const std::function<std::size_t(const void*, std::size_t)>& consumer) override
    {
        auto writer = _writer.load();
        auto reader = _reader.load();
        auto watermark = _watermark.load();
        bool updateWatermark {false};
        size_t bytesAvailable {0U};

        if (reader >= watermark) {
            reader = 0U;
            watermark = capacity() - 1;
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
        // todo: sanity check
        _reader.store(reader + bytesRead);
        if (updateWatermark) {
            // The watermark is only update here when the reader is ahead of the
            // writer. On the other hand, the writer can only update the
            // watermark when it's ahead of the reader. So, there is no race
            // condition here.
            _watermark.store(watermark);
        }
        return true;
    }

    size_t write(const void* data, std::size_t size)
    {
        bool writeSuccessful = write(size, [&](void* buffer, std::size_t bufferSize) {
            return std::memcpy(buffer, data, bufferSize);
        });
        return writeSuccessful ? size : 0U;
    }

    size_t read(void* data, std::size_t size)
    {
        bool readSuccessful = read([&](const void* buffer, std::size_t bufferSize) {
            return std::memcpy(data, buffer, bufferSize);
        });
        return readSuccessful ? size : 0U;
    }

private:
    std::array<uint8_t, SIZE> _buffer {};

    std::atomic<std::size_t> _reader {0U};
    std::atomic<std::size_t> _writer {0U};
    std::atomic<std::size_t> _watermark {SIZE - 1};

};
} // namespace se

#endif // SE_SE_LOG_BUFFER_ATOMICRINGBUFFER_H