/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/buffer/ImmediateBuffer.h"
#include "se-oss/log/format/PrintfFormatter.h"

namespace se_oss {

/**
 * Customization point for logger configuration.
 *
 * Check README for customization instructions.
 *
 * Default log configuration
 * - Printf style
 * - Output to console
 * - Max message length of 128 characters
 */
namespace log_conf {

/*
 * Enable compile time string replacement.
 *
 * Note: You will need to call se_create_resource_database(TARGET my_application)
 * in your CMakeLists.txt to create the actually create the replacements.
 */
// #define SE_OSS_LOG_REPLACE_STRINGS

using Formatter = PrintfFormatter<TimeFormat::ISO8601>;

using Buffer = ImmediateBuffer<128>;

constexpr std::size_t MAX_MESSAGE_LENGTH {128};

constexpr LogLevel MAX_LOG_LEVEL {LogLevel::TRACE};

} // namespace log_conf

} // namespace se_oss