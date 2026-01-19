/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "ILogSink.h"
#include "se-oss/log/ILogFilter.h"

namespace se_oss {

/**
 * Sink wrapper that adds filtering capabilities to any IWriter implementation.
 *
 * This allows you to apply log level filtering or custom filter functions
 * to sinks that only implement IWriter (like ConsoleSink or BufferSink).
 *
 * @tparam T The underlying writer type (must implement IWriter).
 */
template<class T>
class FilteredSink final :
    public ILogSink
{
public:
    static_assert(std::is_base_of<IWriter, T>::value, "Sink must implement IWriter");

    /**
     * Constructs a FilteredSink, forwarding arguments to the underlying sink.
     * @tparam Args Argument types for the underlying sink constructor.
     * @param args Arguments to forward to the underlying sink.
     */
    template<typename... Args>
    explicit FilteredSink(Args&&... args) : _sink{std::forward<Args>(args)...} {}

    ~FilteredSink() override = default;
    FilteredSink(const FilteredSink&) = delete;
    FilteredSink(FilteredSink&&) = delete;
    FilteredSink& operator=(const FilteredSink&) = delete;
    FilteredSink& operator=(FilteredSink&&) = delete;

    void write(const LogMetadata& metadata, const void* data, std::size_t length) override
    {
        if (_filter == nullptr || !_filter(metadata)) {
            return;
        }
        _sink.write(data, length);
    }

    void flush() override
    {
        _sink.flush();
    }

    void setLogLevel(LogLevel level) override
    {
        _filter = [level](LogMetadata metadata) { return metadata.level >= level; };
    }

    void setFilter(std::function<bool(const LogMetadata&)> filter) override
    {
        _filter = filter;
    }

    /**
     * Provides access to the underlying writer.
     * @return Reference to the inner writer.
     */
    T& inner() { return _sink; }

private:
    T _sink;
    std::function<bool(const LogMetadata&)> _filter {[](const LogMetadata&) -> bool { return true; }};
};

} // namespace se_oss_oss
