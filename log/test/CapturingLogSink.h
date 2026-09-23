/*
 * Copyright (c) 2026 Source Engineers GmbH
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/Log.h"
#include "se-oss/log/sink/ILogSink.h"

#include <cstddef>
#include <string>
#include <vector>

namespace se_oss {

/**
 * Test sink that records every log message it receives.
 *
 * The sink decodes the header prepended to each record and stores the level together with the
 * formatted message text, so tests can assert on both. The text is whatever the configured
 * formatter produced, i.e. it includes the timestamp, level and logger-name prefix, so match on
 * it with HasSubstr rather than with an exact comparison.
 *
 * Use LogSinkMock instead when only call expectations matter.
 */
class CapturingLogSink final : public ILogSink
{
public:
    struct Record
    {
        LogLevel level;
        std::string text;
    };

    CapturingLogSink() = default;
    ~CapturingLogSink() override = default;
    CapturingLogSink(const CapturingLogSink&) = delete;
    CapturingLogSink(CapturingLogSink&&) = delete;
    CapturingLogSink& operator=(const CapturingLogSink&) = delete;
    CapturingLogSink& operator=(CapturingLogSink&&) = delete;

    void write(const LogMetadata& metadata, const void* data, std::size_t length) override
    {
        LogHeader header {};
        const void* payload = deserialize(header, data, length);
        if (payload == nullptr) {
            return;
        }

        // Never trust the length in the header beyond what was actually delivered.
        const std::size_t available = length - LogHeader::PACKED_SIZE;
        const std::size_t textLength = (header.messageLength < available) ? header.messageLength : available;

        _records.push_back(Record {metadata.level, std::string(static_cast<const char*>(payload), textLength)});
    }

    void flush() override { }
    void setLogFilterLevel(LogLevel) override { }
    void setCustomLogFilter(const ILogFilter*) override { }

    /**
     * All records received so far, in the order they were written.
     */
    const std::vector<Record>& records() const { return _records; }

    /**
     * Number of records received so far.
     */
    std::size_t count() const { return _records.size(); }

    /**
     * Forget everything received so far.
     */
    void clear() { _records.clear(); }

private:
    std::vector<Record> _records {};
};

}  // namespace se_oss
