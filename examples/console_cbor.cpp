/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */


#include "LogComponents.h"
#include "se-log/Log.h"
#include "se-log/LogRegistry.h"
#include "se-log/format/CborFormatter.h"
#include "se-log/format/StringLookUp.h"
#include "se-log/sink/ConsoleSink.h"

#include <chrono>

// select log formatter
template <>
auto se::logFormatter<>() { return CborFormatter{}; }


int main()
{
    auto shellSink = std::make_unique<se::ConsoleSink>(true);

    auto logRegistry = std::make_unique<se::LogRegistry<LogComponents, LogSinks>>();
    logRegistry->setTimeProvider([]() {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    });
    logRegistry->attachSink(LogSinks::SHELL, std::move(shellSink));

    logRegistry->getSink(LogSinks::SHELL).setLogLevel(se::LogLevel::TRACE);

    se::Logger& logger = logRegistry->createOrGetLogger(LogComponents::CELLULAR);

    logger.setLogLevel(se::LogLevel::TRACE);

    logger.trace("trace");
    logger.debug("debug");
    logger.info("info");
    logger.warn("warn");
    logger.error("error");
    logger.fatal("fatal");

    for (uint32_t i = 0; i < 10; ++i) {
        logger.info("hello %u", i);
    }

    logRegistry->run();

    logger.info("values %u", 1U);
    logger.info("values %u, %u", 1U, 2U);
    logger.info("values %u, %u, %u", 1U, 2U, 3U);
    logger.info("values %u, %u, %u, %u", 1U, 2U, 3U, 4U);
    logger.info("values with floats %f", 42.0F);
    logger.info("values with floats %f, %u", 42.0F, 2U);
    logger.info("values negative %f, %d", -42.0F, -2);
    logger.info("values string %s", "hello");

    logRegistry->run();

    return 0;
}

