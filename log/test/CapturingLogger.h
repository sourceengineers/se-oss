/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "CapturingLogSink.h"
#include "se-oss/log/Log.h"
#include "se-oss/log/LogRegistry.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>

namespace se_oss {

/**
 * One-member test fixture for logging: a default LogRegistry whose sink is a CapturingLogSink,
 * with a fixed time and a Logger to hand to the class under test.
 *
 * Records are checked with contains() or through sink(). In a deferred (AtomicBuffer) log
 * configuration both drain the context first, so pending messages are visible without an
 * explicit distributeMessages() call.
 */
class CapturingLogger final
{
public:
    /**
     * @param name Logger name; appears in the formatted text as `[name]`.
     * @param tag Logger tag.
     */
    explicit CapturingLogger(const char* name = "test", uint8_t tag = 0U) :
        _sink {attachCapturingSink(_registry)},
        _logger {_registry.createLogger(DefaultLogContext::DEFAULT)}
    {
        _registry.setTimeProvider(fixedTime);
        _logger.setName(name);
        _logger.setLogTag(tag);
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
        return *_sink;
    }

    /**
     * The registry, e.g. to create further loggers.
     */
    LogRegistry<>& registry() { return _registry; }

    /**
     * The context of the logger, e.g. to set the log level under test.
     */
    LogContext& context() { return _registry.createOrGetContext(DefaultLogContext::DEFAULT); }

    /**
     * @return true when a record with the given level exists whose text contains substring.
     */
    bool contains(LogLevel level, const char* substring)
    {
        drain();
        const auto& records = _sink->records();
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
        const auto& records = _sink->records();
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
        return _sink->count();
    }

    /**
     * Forget everything recorded so far.
     */
    void clear()
    {
        drain();
        _sink->clear();
    }

private:
    LogRegistry<> _registry {};
    CapturingLogSink* _sink;
    Logger _logger;

    static uint64_t fixedTime() { return 0U; }

    // Attached before the first logger is created, otherwise the registry attaches its ConsoleSink.
    static CapturingLogSink* attachCapturingSink(LogRegistry<>& registry)
    {
        auto sink = std::make_unique<CapturingLogSink>();
        CapturingLogSink* raw = sink.get();
        registry.attachSink(DefaultLogSink::CONSOLE, std::move(sink));
        return raw;
    }

    // No-op with an ImmediateBuffer; moves every pending record to the sink with an AtomicBuffer.
    void drain() { context().distributeMessages(std::numeric_limits<std::size_t>::max()); }
};

}  // namespace se_oss
