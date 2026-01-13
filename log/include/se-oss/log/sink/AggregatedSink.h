/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogSink.h"

#include <memory>
#include <unordered_map>

namespace se_oss {

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

    void setFilter(std::function<bool(const LogMetadata&)> filter) override
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

    void attachSink(TSink id, std::unique_ptr<ILogSink> sink)
    {
        _sinks.emplace(id, std::move(sink));
    }

    ILogSink& getSink(TSink sink) { return *_sinks.at(sink); }

    [[nodiscard]] bool empty() const { return _sinks.empty(); }

private:
    std::unordered_map<TSink, std::unique_ptr<ILogSink>> _sinks {};
    std::function<bool(const LogMetadata&)> _filter {};
};

} // namespace se
