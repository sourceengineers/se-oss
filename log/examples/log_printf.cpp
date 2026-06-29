/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#include "LogConf.h"
#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"
#include "se-oss/log/sink/ConsoleSink.h"

int main()
{
    auto consoleSink = std::make_unique<se_oss::ConsoleSink>();

    auto logRegistry = std::make_unique<se_oss::LogRegistry<MyLogContext, MyLogSink>>();
    logRegistry->attachSink(MyLogSink::SHELL, std::move(consoleSink));
    logRegistry->getSink(MyLogSink::SHELL).setLogLevel(se_oss::LogLevel::TRACE);

    se_oss::Logger log = logRegistry->createLogger(MyLogContext::CELLULAR);

    log.setLogLevel(se_oss::LogLevel::TRACE);
    logRegistry->createOrGetContext(MyLogContext::CELLULAR).setLogLevel(se_oss::LogLevel::TRACE);

    LOG_TRACE(log, "trace");
    LOG_DEBUG(log, "debug");
    LOG_INFO(log, "info");
    LOG_WARN(log, "warn");
    LOG_ERROR(log, "error");
    LOG_FATAL(log, "fatal");

    for (uint32_t i = 0; i < 10; ++i) {
        LOG_INFO(log, "hello %u", i);
    }

    logRegistry->distributeMessages();

    LOG_INFO(log, "values %u", 1U);
    LOG_INFO(log, "values %u, %u", 1U, 2U);
    LOG_INFO(log, "values %u, %u, %u", 1U, 2U, 3U);
    LOG_INFO(log, "values %u, %u, %u, %u", 1U, 2U, 3U, 4U);
    LOG_INFO(log, "values with floats %f", 42.0F);
    LOG_INFO(log, "values with floats %f, %u", 42.0F, 2U);
    LOG_INFO(log, "values negative %f, %d", -42.0F, -2);
    LOG_INFO(log, "values string %s", "hello");

    logRegistry->distributeMessages();

    return 0;
}
