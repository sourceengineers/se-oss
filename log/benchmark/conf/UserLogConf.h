/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

// Log configuration for the benchmarks: deferred logging through an AtomicBuffer and no timestamp
// formatting, so that the numbers reflect the logging pipeline rather than strftime.
//
// The formatter is selected per executable: FORMAT_CBOR, FORMAT_CBOR_REPLACE_STRINGS, or printf by
// default. Buffer and message length are identical in every branch, as they have to be: LogContext
// embeds the buffer, and the library is compiled once and shared by all benchmark executables.

#include "se-oss/log/buffer/AtomicBuffer.h"
#include "se-oss/log/format/PrintfFormatter.h"
#if defined FORMAT_CBOR || defined FORMAT_CBOR_REPLACE_STRINGS
#include "se-oss/log/format/CborFormatter.h"
#endif

namespace se_oss {
namespace log_conf {

constexpr std::size_t BUFFER_SIZE {2048U};
constexpr std::size_t MAX_MESSAGE_LENGTH {128U};
constexpr LogLevel MAX_LOG_LEVEL {LogLevel::TRACE};

using Buffer = AtomicBuffer<BUFFER_SIZE>;

#if defined FORMAT_CBOR_REPLACE_STRINGS
#define SE_OSS_LOG_REPLACE_STRINGS
using Formatter = CborFormatter;
#elif defined FORMAT_CBOR
using Formatter = CborFormatter;
#else
using Formatter = PrintfFormatter<TimeFormat::NONE>;
#endif

}  // namespace log_conf
}  // namespace se_oss
