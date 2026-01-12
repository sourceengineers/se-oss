/*
* Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "LogComponents.h"
#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/format/CborFormatter.h"
#include "se-oss/log/sink/ConsoleSink.h"
#include "se-oss/log/sink/FilteredSink.h"

#include <chrono>

// Configure logger
constexpr std::size_t LOG_BUFFER_SIZE {2048U};
constexpr std::size_t LOG_MAX_MESSAGE_LENGTH {128U};

template <>
auto se_oss::logConf<>()
{
    return LogConf<CborFormatter, AtomicBuffer<LOG_BUFFER_SIZE>, LOG_MAX_MESSAGE_LENGTH>{};
}

int main()
{
    auto shellSink = std::make_unique<se_oss::FilteredSink<se_oss::ConsoleSink>>(true);

    auto logRegistry = std::make_unique<se_oss::LogRegistry<LogComponents, LogSinks>>();
    logRegistry->setTimeProvider([]() {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    });
    logRegistry->attachSink(LogSinks::SHELL, std::move(shellSink));
    logRegistry->getSink(LogSinks::SHELL).setLogLevel(se_oss::LogLevel::TRACE);

    se_oss::Logger& logger = logRegistry->createOrGetLogger(LogComponents::CELLULAR);

    logger.setLogLevel(se_oss::LogLevel::TRACE);

    LOG_TRACE(logger, "trace");
    LOG_DEBUG(logger, "debug");
    LOG_INFO(logger, "info");
    LOG_WARN(logger, "warn");
    LOG_ERROR(logger, "error");
    LOG_FATAL(logger, "fatal");

    for (uint32_t i = 0; i < 10; ++i) {
        LOG_INFO(logger, "hello %u", i);
    }

    logRegistry->distributeMessages();

    LOG_INFO(logger, "values %u", 1U);
    LOG_INFO(logger, "values %u, %u", 1U, 2U);
    LOG_INFO(logger, "values %u, %u, %u", 1U, 2U, 3U);
    LOG_INFO(logger, "values %u, %u, %u, %u", 1U, 2U, 3U, 4U);
    LOG_INFO(logger, "values with floats %f", 42.0F);
    LOG_INFO(logger, "values with floats %f, %u", 42.0F, 2U);
    LOG_INFO(logger, "values negative %f, %d", -42.0F, -2);
    LOG_INFO(logger, "values string %s", "hello");

    logRegistry->distributeMessages();

    return 0;
}
