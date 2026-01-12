/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "Log.h"
#include "sink/AggregatedSink.h"
#include "sink/ConsoleSink.h"
#include "sink/FilteredSink.h"

#include <cassert>
#include <memory>
#include <unordered_map>

namespace se_oss {

enum class DefaultLogComponents : uint8_t
{
    DEFAULT = 0
};

constexpr const char* toString(DefaultLogComponents component)
{
    (void) component;
    return "default";
}

enum class DefaultLogSinks : uint8_t
{
    CONSOLE = 0
};

template<typename TComponent = DefaultLogComponents, typename TSink = DefaultLogSinks>
class LogRegistry
{
public:
    LogRegistry() = default;
    ~LogRegistry() = default;
    LogRegistry(const LogRegistry&) = delete;
    LogRegistry(LogRegistry&&) = delete;
    LogRegistry& operator=(const LogRegistry&) = delete;
    LogRegistry& operator=(LogRegistry&&) = delete;


    Logger& createOrGetLogger(TComponent component)
    {
        assert(checkSinkHandler<TSink>());

        if (_logger.find(component) == _logger.end()) {
            _logger.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(component),
                std::forward_as_tuple(static_cast<uint8_t>(component), toString(component), _sinkHandler, [this](){return getTime();})
            );
        }
        return _logger.at(component);
    }

    void setTimeProvider(const std::function<uint64_t()>& provider) { _timeProvider = provider; }

    void attachSink(TSink id, std::unique_ptr<ILogSink> sink)
    {
        assert(sink != nullptr);
        _sinkHandler.attachSink(id, std::move(sink));
    }

    ILogSink& getSink(TSink sink) { return _sinkHandler.getSink(sink); }

    void distributeMessages()
    {
        for (auto& logger : _logger) {
            logger.second.distributeMessages();
        }
    }

private:
    AggregatedSink<TSink> _sinkHandler {};
    std::unordered_map<TComponent, Logger> _logger {};
    std::function<uint64_t()> _timeProvider;

    uint64_t getTime()
    {
        if (_timeProvider) {
            return _timeProvider();
        } else {
            return UINT64_MAX;
        }
    }

    template <typename T = TSink>
    std::enable_if_t<!std::is_same<T, DefaultLogSinks>::value, bool> checkSinkHandler()
    {
        return !_sinkHandler.empty();
    }

    template<typename  T = DefaultLogSinks>
    std::enable_if_t<std::is_same<T, DefaultLogSinks>::value, bool> checkSinkHandler()
    {
        // In case of the default logger create the console sink ad hoc
        if (_sinkHandler.empty()) {
            attachSink(DefaultLogSinks::CONSOLE, std::make_unique<FilteredSink<ConsoleSink>>());
        }
        return true;
    }
};

} // namespace se
