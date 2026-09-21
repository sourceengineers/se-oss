/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

// Benchmarks the logging hot path: format a message into the context's buffer and hand it on to a
// sink that discards it. The formatter is selected per executable through the FORMAT_* definitions
// understood by conf/UserLogConf.h.

#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/sink/FilteredSink.h"
#include "se-oss/log/sink/NullSink.h"

#include <benchmark/benchmark.h>

#include <cstddef>
#include <cstdint>
#include <memory>

namespace {

constexpr std::uint32_t MESSAGES_PER_ITERATION {1000U};
// Well below what the 2048 byte buffer holds, so nothing is dropped between distributions.
constexpr std::uint32_t MESSAGES_PER_DISTRIBUTION {10U};

se_oss::LogRegistry<> registry {};
std::unique_ptr<se_oss::Logger> logger {};

void setup(const benchmark::State&)
{
    if (logger == nullptr) {
        // Replace the console sink the registry would otherwise attach with one that discards everything.
        registry.attachSink(
            se_oss::DefaultLogSink::CONSOLE, std::make_unique<se_oss::FilteredSink<se_oss::NullSink>>()
        );
        logger = std::make_unique<se_oss::Logger>(registry.createLogger(se_oss::DefaultLogContext::DEFAULT));
    }
}

void finish(benchmark::State& state)
{
    state.SetItemsProcessed(state.iterations() * MESSAGES_PER_ITERATION);
    if (logger->statistics().droppedMessages != 0U) {
        state.SkipWithError("messages were dropped: the buffer is too small for the distribution interval");
    }
}

void log_uint(benchmark::State& state)
{
    for (auto _ : state) {
        for (std::uint32_t i = 1U; i <= MESSAGES_PER_ITERATION; ++i) {
            LOG_INFO((*logger), "formatting a value %u", i);
            if (i % MESSAGES_PER_DISTRIBUTION == 0U) {
                registry.distributeMessages();
            }
        }
    }
    finish(state);
}

void log_float(benchmark::State& state)
{
    for (auto _ : state) {
        for (std::uint32_t i = 1U; i <= MESSAGES_PER_ITERATION; ++i) {
            LOG_INFO((*logger), "formatting a value %f", static_cast<float>(i));
            if (i % MESSAGES_PER_DISTRIBUTION == 0U) {
                registry.distributeMessages();
            }
        }
    }
    finish(state);
}

void log_three_values(benchmark::State& state)
{
    // Three arguments: the closure that used to exceed std::function's inline storage on every target.
    for (auto _ : state) {
        for (std::uint32_t i = 1U; i <= MESSAGES_PER_ITERATION; ++i) {
            LOG_INFO((*logger), "values %u, %d, %zu", i, -static_cast<int>(i), static_cast<std::size_t>(i));
            if (i % MESSAGES_PER_DISTRIBUTION == 0U) {
                registry.distributeMessages();
            }
        }
    }
    finish(state);
}

}  // namespace

BENCHMARK(log_uint)->Setup(setup);
BENCHMARK(log_float)->Setup(setup);
BENCHMARK(log_three_values)->Setup(setup);

BENCHMARK_MAIN();
