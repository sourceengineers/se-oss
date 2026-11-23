/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

// Compiled, never linked, by the LogFormatCheck tests: a log call whose argument does not match its format. With
// SE_OSS_LOG_CHECK_PRINTF_FORMAT and -Werror=format it must fail to compile, without it (the default) it must compile.

#include "se-oss/log/Log.h"

#include <cstdint>

void logWithMismatchedFormat(se_oss::Logger& logger, uint32_t count);

void logWithMismatchedFormat(se_oss::Logger& logger, uint32_t count) { LOG_INFO(logger, "count %s", count); }
