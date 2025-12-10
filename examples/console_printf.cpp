/*
* Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */
// copied from zephyr repo: https://github.com/zephyrproject-rtos/zephyr/blob/main/kernel/device.c



#include "LogComponents.h"
#include "se-log/Log.h"
#include "se-log/LogRegistry.h"
#include "../include/se-log/StringLookUp.h"
#include "se-log/sink/ConsoleSink.h"

#include <chrono>

// select log formatter
template <>
auto se_oss::logFormatter<>() { return PrintfFormatter{}; }

int main()
{
    auto shellSink = std::make_unique<se_oss::ConsoleSink>();
    auto fileSink = std::make_unique<se_oss::ConsoleSink>();

    auto logRegistry = std::make_unique<se_oss::LogRegistry<LogComponents, LogSinks>>();
    logRegistry->setTimeProvider([]() {
        return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    });
    logRegistry->attachSink(LogSinks::SHELL, std::move(shellSink));
    // logRegistry->attachSink(LogSinks::FILE, std::move(fileSink))

    logRegistry->getSink(LogSinks::SHELL).setLogLevel(se_oss::LogLevel::TRACE);

    se_oss::Logger& loggerCellular = logRegistry->createOrGetLogger(LogComponents::CELLULAR);
    se_oss::Logger& loggerWuff = logRegistry->createOrGetLogger(LogComponents::WUFF);

    loggerCellular.setLogLevel(se_oss::LogLevel::TRACE);

    loggerCellular.trace("trace");
    loggerCellular.debug("debug");
    loggerCellular.info("info");
    loggerCellular.warn("warn");
    loggerCellular.error("error");
    loggerCellular.fatal("fatal");

    for (uint32_t i = 0; i < 10; ++i) {
        loggerCellular.info("hello %u", i);
    }

    logRegistry->createOrGetLogger(LogComponents::WUFF).setLogLevel(se_oss::LogLevel::WARN);

    loggerWuff.warn("wuff");

    se_oss::Logger& loggerStorage = logRegistry->createOrGetLogger(LogComponents::STORAGE);
    loggerStorage.info("values %u", 1U);
    loggerStorage.info("values %u, %u", 1U, 2U);
    loggerStorage.info("values %u, %u, %u", 1U, 2U, 3U);
    loggerStorage.info("values %u, %u, %u, %u", 1U, 2U, 3U, 4U);
    loggerStorage.info("values with floats %f", 42.0F);
    loggerStorage.info("values with floats %f, %u", 42.0F, 2U);
    loggerStorage.info("values negative %f, %d", -42.0F, -2);

    // string replacement demo
    loggerStorage.info("string replacement id %u", se_oss::getStringId<decltype("hello world"_text)>());

    logRegistry->distributeMessages();
    logRegistry->distributeMessages();

    return 0;
}
