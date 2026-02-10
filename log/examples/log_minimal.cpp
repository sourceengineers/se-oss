/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"

int main()
{
    auto logRegistry = std::make_unique<se_oss::LogRegistry<>>();

    se_oss::Logger& logger = logRegistry->createOrGetLogger(se_oss::DefaultLogComponents::DEFAULT);

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

    LOG_INFO(logger, "values %u", 1U);
    LOG_INFO(logger, "values %u, %u", 1U, 2U);
    LOG_INFO(logger, "values %u, %u, %u", 1U, 2U, 3U);
    LOG_INFO(logger, "values %u, %u, %u, %u", 1U, 2U, 3U, 4U);
    LOG_INFO(logger, "values with floats %f", 42.0F);
    LOG_INFO(logger, "values with floats %f, %u", 42.0F, 2U);
    LOG_INFO(logger, "values negative %f, %d", -42.0F, -2);
    LOG_INFO(logger, "values string %s", "hello");

    return 0;
}
