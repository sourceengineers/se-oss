/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/format/CborFormatter.h"
#include "se-oss/log/sink/NullSink.h"

#include <benchmark/benchmark.h>

#include <iostream>

constexpr std::size_t LOG_BUFFER_SIZE {2048U};
constexpr std::size_t LOG_MAX_MESSAGE_LENGTH {128U};

#if defined BENCH_PRINTF
template<>
auto se_oss::logConf<>()
{
    return LogConf<PrintfFormatter, AtomicBuffer<LOG_BUFFER_SIZE>, LOG_MAX_MESSAGE_LENGTH> {};
}
#elif defined BENCH_CBOR
template<>
auto se_oss::logConf<>()
{
    return LogConf<CborFormatter, AtomicBuffer<LOG_BUFFER_SIZE>, LOG_MAX_MESSAGE_LENGTH> {};
}
#endif

static se_oss::Logger* log {nullptr};
static se_oss::LogRegistry<> log_registry {};

static void setup(const benchmark::State&)
{
    if (log == nullptr) {
        // override default sink
        log_registry.attachSink(
            se_oss::DefaultLogSinks::CONSOLE,
            std::make_unique<se_oss::FilteredSink<se_oss::NullSink>>()
        );
        log = &log_registry.createOrGetLogger(se_oss::DefaultLogComponents::DEFAULT);
    }
}

static void log_uint(benchmark::State& state)
{
    for (auto _ : state) {
        for (int i = 1; i <= 1000; i++) {
            LOG_INFO(*log, "formatting a value %u", i);
            if (i % 10 == 0) {
                log_registry.distributeMessages();
            }
        }
    }
}

static void log_float(benchmark::State& state)
{
    for (auto _ : state) {
        for (int i = 1; i <= 1000; i++) {
            LOG_INFO(*log, "formatting a value %f", static_cast<float>(i));
            if (i % 10 == 0) {
                log_registry.distributeMessages();
            }
        }
    }
}

BENCHMARK(log_uint)->Setup(setup);
BENCHMARK(log_float)->Setup(setup);

BENCHMARK_MAIN();
