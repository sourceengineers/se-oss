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

/**
 * Default components for logging.
 */
enum class DefaultLogComponents : uint8_t
{
    DEFAULT = 0 /**< The default component. */
};

constexpr const char* toString(DefaultLogComponents component)
{
    (void) component;
    return "default";
}

/**
 * Default sinks for logging.
 */
enum class DefaultLogSinks : uint8_t
{
    CONSOLE = 0 /**< The default console sink. */
};

/**
 * Registry for managing loggers and sinks.
 *
 * This class acts as a central hub for managing different log components (sources)
 * and sinks (destinations). It handles the creation and retrieval of loggers and
 * the attachment of sinks.
 *
 * @tparam TComponent Enum type representing the log components.
 * @tparam TSink Enum type representing the log sinks.
 */
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


    /**
     * Creates or retrieves a logger for a specific component.
     *
     * If the logger for the given component does not exist, it is created.
     *
     * @note this function will allocate memory when a logger was not yet
     *       created, i.e., on the first call only.
     *
     * @param component The component identifier.
     * @return Reference to the Logger instance.
     */
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

    /**
     * Sets the time provider function.
     * @param provider A function returning the current time in microseconds.
     */
    void setTimeProvider(const std::function<uint64_t()>& provider) { _timeProvider = provider; }

    /**
     * Attaches a sink to a specific sink ID.
     * @param id The sink identifier.
     * @param sink Unique pointer to the sink instance.
     */
    void attachSink(TSink id, std::unique_ptr<ILogSink> sink)
    {
        assert(sink != nullptr);
        _sinkHandler.attachSink(id, std::move(sink));
    }

    /**
     * Retrieves a sink by its ID.
     * @param sink The sink identifier.
     * @return Reference to the ILogSink instance.
     */
    ILogSink& getSink(TSink sink) { return _sinkHandler.getSink(sink); }

    /**
     * Distributes messages for all registered loggers.
     *
     * This iterates over all loggers and calls their distributeMessages method.
     */
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
    std::enable_if_t<!std::is_same<T, DefaultLogSinks>::value, bool> checkSinkHandler() const
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

} // namespace se_oss
