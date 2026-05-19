/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/buffer/AtomicBuffer.h"
#include "se-oss/log/format/CborFormatter.h"
#include "se-oss/log/format/PrintfFormatter.h"

namespace se_oss {
namespace log_conf {

constexpr std::size_t BUFFER_SIZE {2048U};
constexpr std::size_t MAX_MESSAGE_LENGTH {128U};

using Buffer = AtomicBuffer<BUFFER_SIZE>;
constexpr LogLevel MAX_LOG_LEVEL {LogLevel::TRACE};

#if defined FORMAT_CBOR

using Formatter = CborFormatter;
#elif defined FORMAT_CBOR_REPLACE_STRINGS

#define SE_OSS_LOG_REPLACE_STRINGS
using Formatter = CborFormatter;

#else

using Formatter = PrintfFormatter<TimeFormat::ISO8601>;

#endif

} // namespace log_conf
} // namespace se_oss