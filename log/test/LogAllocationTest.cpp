/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

// Regression test for the log hot path: a log call must never allocate, whatever the message
// carries. The global allocation functions are replaced for the whole test executable and counted.
// gtest itself allocates outside the measured windows, so every test resets the counter right
// before the call under test and reads it right after, before any expectation runs.

#include <se-oss/log/Log.h>
#include <se-oss/log/LogContext.h>
#include <se-oss/log/buffer/AtomicBuffer.h>
#include <se-oss/log/sink/FilteredSink.h>
#include <se-oss/log/sink/NullSink.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>

#include <gtest/gtest.h>

namespace {

std::atomic<std::size_t> g_allocations {0U};

}  // namespace

void* operator new(std::size_t size)
{
    ++g_allocations;
    void* memory = std::malloc(size == 0U ? 1U : size);
    if (memory == nullptr) {
        throw std::bad_alloc();
    }
    return memory;
}

void operator delete(void* memory) noexcept { std::free(memory); }

void operator delete(void* memory, std::size_t) noexcept { std::free(memory); }

class LogAllocationTest : public ::testing::Test
{
protected:
    static void resetCounter() { g_allocations.store(0U); }
    static std::size_t allocations() { return g_allocations.load(); }

    se_oss::FilteredSink<se_oss::NullSink> _sink {};
    se_oss::LogContext _context {0U, "alloc", _sink, []() -> uint64_t { return 0U; }};
    se_oss::Logger _logger {_context};
};

TEST_F(LogAllocationTest, TheCounterSeesAllocations)
{
    // Sanity check of the guard itself: without this, every other test here would pass trivially.
    resetCounter();
    auto* allocated = new int {1};
    const std::size_t seen = allocations();
    delete allocated;

    EXPECT_EQ(1U, seen);
}

TEST_F(LogAllocationTest, LoggingWithoutArgumentsDoesNotAllocate)
{
    resetCounter();
    LOG_INFO(_logger, "plain message");
    const std::size_t seen = allocations();

    EXPECT_EQ(0U, seen);
}

TEST_F(LogAllocationTest, LoggingWithOneArgumentDoesNotAllocate)
{
    // Three captured references (record, format, one value): 12 bytes on a 32-bit target, which is
    // already more than std::function stores inline there.
    resetCounter();
    LOG_WARN(_logger, "value %d", 42);
    const std::size_t seen = allocations();

    EXPECT_EQ(0U, seen);
}

TEST_F(LogAllocationTest, LoggingWithThreeArgumentsDoesNotAllocate)
{
    resetCounter();
    LOG_ERROR(_logger, "%d %u %zu", -1, 2U, std::size_t {3U});
    const std::size_t seen = allocations();

    EXPECT_EQ(0U, seen);
}

TEST_F(LogAllocationTest, LoggingWithSixArgumentsDoesNotAllocate)
{
    // Eight captured references: 64 bytes on a 64-bit host, well above any small-buffer size, so
    // this fails against a std::function based hot path on the host as well, not only on target.
    resetCounter();
    LOG_DEBUG(_logger, "%d %d %d %d %d %s", 1, 2, 3, 4, 5, "six");
    const std::size_t seen = allocations();

    EXPECT_EQ(0U, seen);
}

TEST_F(LogAllocationTest, FilteredMessagesDoNotAllocateEither)
{
    _context.setLogLevel(se_oss::LogLevel::OFF);

    resetCounter();
    LOG_FATAL(_logger, "%d %d %d %d", 1, 2, 3, 4);
    const std::size_t seen = allocations();

    EXPECT_EQ(0U, seen);
}

TEST_F(LogAllocationTest, AtomicBufferWriteAndReadDoNotAllocate)
{
    // The deferred buffer is a compile-time configuration, so it is exercised directly. The
    // closures copy a 40-byte payload, which puts them above every small-buffer size by construction.
    se_oss::AtomicBuffer<256U> buffer;
    std::array<std::uint8_t, 40U> payload {};
    payload.fill(0xABU);
    bool payloadMatches {false};

    resetCounter();
    const bool written = buffer.write(payload.size(), [payload](void* destination, std::size_t size) {
        const std::size_t length = std::min(size, payload.size());
        std::memcpy(destination, payload.data(), length);
        return length;
    });
    const bool read = buffer.read([payload, &payloadMatches](const void* source, std::size_t size) {
        const std::size_t length = std::min(size, payload.size());
        payloadMatches = std::memcmp(source, payload.data(), length) == 0;
        return length;
    });
    const std::size_t seen = allocations();

    EXPECT_TRUE(written);
    EXPECT_TRUE(read);
    EXPECT_TRUE(payloadMatches);
    EXPECT_EQ(0U, seen);
}
