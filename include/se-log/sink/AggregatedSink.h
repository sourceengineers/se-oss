/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_LOG_AGGREGATEDSINK_H
#define SE_LOG_AGGREGATEDSINK_H
#include "ILogSink.h"

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

    void write(LogMetadata metadata, const void* data, size_t length) override
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
    }

    void setComponentLogLevel(LogLevel level, uint8_t componentId) override
    {
        for (const auto& sink : _sinks) {
            sink.second->setComponentLogLevel(level, componentId);
        }
    }

    void attachSink(TSink id, std::unique_ptr<ILogSink> sink)
    {
        _sinks.emplace(id, std::move(sink));
    }

    ILogSink& getSink(TSink sink) { return *_sinks.at(sink); }

    bool empty() const { return _sinks.empty(); }

private:
    std::unordered_map<TSink, std::unique_ptr<ILogSink>> _sinks {};
};

} // namespace se

#endif // SE_LOG_AGGREGATEDSINK_H
