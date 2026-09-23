/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "CapturingLogSink.h"
#include "FixedTimeProvider.h"
#include "se-oss/log/Log.h"
#include "se-oss/log/LogContext.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

namespace se_oss {

/**
 * One-member test fixture for logging: bundles a CapturingLogSink, a LogContext with a fixed
 * time and a Logger to hand to the class under test.
 *
 * Records are checked with contains() or through sink(). In a deferred (AtomicBuffer) log
 * configuration both drain the context first, so pending messages are visible without an
 * explicit distributeMessages() call.
 */
class CapturingLogger final
{
public:
    /**
     * @param name Context name; appears in the formatted text as `[name]`.
     * @param tag Context tag.
     */
    explicit CapturingLogger(const char* name = "test", uint8_t tag = 0U) :
        _context {tag, name, _sink, _timeProvider},
        _logger {_context}
    {
    }

    ~CapturingLogger() = default;
    CapturingLogger(const CapturingLogger&) = delete;
    CapturingLogger(CapturingLogger&&) = delete;
    CapturingLogger& operator=(const CapturingLogger&) = delete;
    CapturingLogger& operator=(CapturingLogger&&) = delete;

    /**
     * Logger to hand to the class under test.
     */
    Logger& logger() { return _logger; }

    /**
     * Recorded messages (level and formatted text), pending messages drained.
     */
    CapturingLogSink& sink()
    {
        drain();
        return _sink;
    }

    /**
     * The context, e.g. to set the log level under test.
     */
    LogContext& context() { return _context; }

    /**
     * @return true when a record with the given level exists whose text contains substring.
     */
    bool contains(LogLevel level, const char* substring)
    {
        drain();
        const auto& records = _sink.records();
        return std::any_of(records.begin(), records.end(), [level, substring](const CapturingLogSink::Record& record) {
            return (record.level == level) && (record.text.find(substring) != std::string::npos);
        });
    }

    /**
     * @return true when a record of any level exists whose text contains substring.
     */
    bool contains(const char* substring)
    {
        drain();
        const auto& records = _sink.records();
        return std::any_of(records.begin(), records.end(), [substring](const CapturingLogSink::Record& record) {
            return record.text.find(substring) != std::string::npos;
        });
    }

    /**
     * Number of records received so far, pending messages drained.
     */
    std::size_t count()
    {
        drain();
        return _sink.count();
    }

    /**
     * Forget everything recorded so far.
     */
    void clear()
    {
        drain();
        _sink.clear();
    }

private:
    static constexpr uint64_t FIXED_TIMESTAMP {0U};

    CapturingLogSink _sink {};
    FixedTimeProvider _timeProvider {FIXED_TIMESTAMP};
    LogContext _context;
    Logger _logger;

    // No-op with an ImmediateBuffer; moves every pending record to the sink with an AtomicBuffer.
    void drain() { _context.distributeMessages(std::numeric_limits<std::size_t>::max()); }
};

}  // namespace se_oss
