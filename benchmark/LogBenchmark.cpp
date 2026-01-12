/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/format/CborFormatter.h"
#include "se-oss/log/sink/NullSink.h"

#include <iostream>
#include <benchmark/benchmark.h>


constexpr std::size_t LOG_BUFFER_SIZE {2048U};
constexpr std::size_t LOG_MAX_MESSAGE_LENGTH {128U};

#if defined BENCH_PRINTF
template <>
auto se_oss::logConf<>()
{
    return LogConf<PrintfFormatter, AtomicBuffer<LOG_BUFFER_SIZE>, LOG_MAX_MESSAGE_LENGTH>{};
}
#elif defined BENCH_CBOR
template <>
auto se_oss::logConf<>()
{
    return LogConf<CborFormatter, AtomicBuffer<LOG_BUFFER_SIZE>, LOG_MAX_MESSAGE_LENGTH>{};
}
#endif

static se_oss::Logger* LOG {nullptr};
static se_oss::LogRegistry<> LOG_REGISTRY {};

static void setup(const benchmark::State&)
{
    if (LOG == nullptr) {
        // override default sink
        LOG_REGISTRY.attachSink(se_oss::DefaultLogSinks::CONSOLE, std::make_unique<se_oss::FilteredSink<se_oss::NullSink>>());
        LOG = &LOG_REGISTRY.createOrGetLogger(se_oss::DefaultLogComponents::DEFAULT);
    }
}

static void log_uint(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (int i = 1; i <= 1000; i++)
        {
            LOG_INFO((*LOG), "formatting a value %u", i);
            if (i % 10 == 0) {
                LOG_REGISTRY.distributeMessages();
            }
        }
    }
}

static void log_float(benchmark::State& state)
{
    for (auto _ : state)
    {
        for (int i = 1; i <= 1000; i++)
        {
            LOG_INFO((*LOG), "formatting a value %f", static_cast<float>(i));
            if (i % 10 == 0) {
                LOG_REGISTRY.distributeMessages();
            }
        }
    }
}

BENCHMARK(log_uint)->Setup(setup);
BENCHMARK(log_float)->Setup(setup);

BENCHMARK_MAIN();