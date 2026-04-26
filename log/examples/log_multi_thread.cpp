/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/sink/ConsoleSink.h"

#include <mutex>
#include <string>
#include <thread>
#include <vector>

enum class ThreadId
{
    IncrementNumberGenerator,
    ExcuseGenerator,
    LogAggregator,
};

constexpr const char* toString(const ThreadId threadId)
{
    switch (threadId) {
        case ThreadId::IncrementNumberGenerator: return "num";
        case ThreadId::ExcuseGenerator: return "exc";
        case ThreadId::LogAggregator: return "log";
        default: return "unknown";
    }
}

enum class LogSinkId
{
    Console,
};

se_oss::LogRegistry<ThreadId, LogSinkId> log_registry;
std::mutex log_registry_mutex;

std::atomic_bool shall_exit;

se_oss::Logger create_logger_mt_safe(const ThreadId tid)
{
    std::lock_guard<std::mutex> lock(log_registry_mutex);
    se_oss::Logger log = log_registry.createLogger(tid);
    return log;
}

void increment_number(const ThreadId tid)
{
    int num = 0;
    auto log = create_logger_mt_safe(tid);
    while (!shall_exit.load()) {
        LOG_INFO(log, "Current number: %d", num++);
        std::this_thread::sleep_for(std::chrono::milliseconds(7));
    }
}

std::vector<std::string> excuses = {
    "Temporary Array Interruption",
    "Intermittent Systems Destabilization",
    "Partial Hardware Destruction",
};

void excuse(const ThreadId tid)
{
    auto excuse_it = excuses.begin();
    auto log = create_logger_mt_safe(tid);
    while (!shall_exit.load()) {
        LOG_INFO(log, "%s", excuse_it++->c_str());
        if (excuse_it == excuses.end()) {
            excuse_it = excuses.begin();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(11));
    }
}

void aggregate_logs(const ThreadId tid)
{
    auto log = create_logger_mt_safe(tid);
    while (!shall_exit.load()) {
        LOG_TRACE(log, "Draining log messages");
        {
            std::lock_guard<std::mutex> lock(log_registry_mutex);
            log_registry.distributeMessages();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv)
{
    {
        auto console_sink = std::make_unique<se_oss::FilteredSink<se_oss::ConsoleSink>>();
        console_sink->setLogLevel(se_oss::LogLevel::TRACE);
        std::lock_guard<std::mutex> lock(log_registry_mutex);
        log_registry.attachSink(LogSinkId::Console, std::move(console_sink));
    }

    std::thread increment_number_generator(increment_number, ThreadId::IncrementNumberGenerator);
    std::thread excuse_generator(excuse, ThreadId::ExcuseGenerator);
    std::thread log_aggregator(aggregate_logs, ThreadId::LogAggregator);

    std::this_thread::sleep_for(std::chrono::milliseconds(100));

    shall_exit.store(true);
    increment_number_generator.join();
    excuse_generator.join();
    log_aggregator.join();
    return 0;
}
