/*
 * Copyright (c) 2025 Source Engineers GmbH, Switzerland
 * Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogSink.h"

#include <memory>
#include <unordered_map>

namespace se_oss {

/**
 * Sink that aggregates multiple sinks and writes to all of them.
 *
 * This allows logging to multiple destinations simultaneously (e.g., console and file).
 * Each sink is identified by a unique ID of type TSink.
 *
 * @tparam TSink Enum type for sink identifiers.
 */
template<typename TSink>
class AggregatedSink final : public ILogSink
{
public:
    AggregatedSink() = default;
    ~AggregatedSink() override = default;
    AggregatedSink(const AggregatedSink&) = delete;
    AggregatedSink(AggregatedSink&&) = delete;
    AggregatedSink& operator=(const AggregatedSink&) = delete;
    AggregatedSink& operator=(AggregatedSink&&) = delete;

    void write(const LogMetadata& metadata, const void* data, size_t length) override
    {
        if (data == nullptr || length == 0) {
            return;
        }
        for (const auto& sink : _sinks) {
            sink.second->write(metadata, data, length);
        }
    }

    void flush() override
    {
        for (const auto& sink : _sinks) {
            sink.second->flush();
        }
    }

    void setLogLevel(LogLevel level) override
    {
        for (const auto& sink : _sinks) {
            sink.second->setLogLevel(level);
        }
        _filter = nullptr;
    }

    void setFilter(LogFilterFunction filter) override
    {
        _filter = filter;
        for (const auto& sink : _sinks) {
            sink.second->setFilter([this](const LogMetadata& metadata) -> bool {
                if (_filter == nullptr) {
                    return false;
                }
                return _filter(metadata);
            });
        }
    }

    /**
     * Attaches a sink to this aggregator.
     * @param id The unique identifier for this sink.
     * @param sink The sink to attach.
     */
    void attachSink(TSink id, std::unique_ptr<ILogSink> sink) { _sinks.emplace(id, std::move(sink)); }

    /**
     * Retrieves a sink by its ID.
     * @param sink The sink identifier.
     * @return Reference to the sink.
     */
    ILogSink& getSink(TSink sink) { return *_sinks.at(sink); }

    /**
     * Checks if the aggregator has no sinks attached.
     * @return True if empty, false otherwise.
     */
    bool empty() const { return _sinks.empty(); }

private:
    std::unordered_map<TSink, std::unique_ptr<ILogSink>> _sinks {};
    LogFilterFunction _filter {};
};

}  // namespace se_oss
