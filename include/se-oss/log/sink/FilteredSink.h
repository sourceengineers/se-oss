/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_OSS_LOG_SINK_FILTEREDSINK_H
#define SE_SE_OSS_LOG_SINK_FILTEREDSINK_H

#include "ILogSink.h"
#include "se-oss/log/ILogFilter.h"

namespace se_oss {

template<class T>
class FilteredSink final :
    public ILogSink
{
public:
    static_assert(std::is_base_of<IWriter, T>::value, "Sink must implement IWriter");

    template<typename... Args>
    explicit FilteredSink(Args&&... args) : _sink{std::forward<Args>(args)...} {}

    ~FilteredSink() override = default;
    FilteredSink(const FilteredSink&) = delete;
    FilteredSink(FilteredSink&&) = delete;
    FilteredSink& operator=(const FilteredSink&) = delete;
    FilteredSink& operator=(FilteredSink&&) = delete;

    void write(LogMetadata metadata, const void* data, std::size_t length) override
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

    void setFilter(std::function<bool(LogMetadata)> filter) override
    {
        _filter = filter;
    }

private:
    T _sink;
    std::function<bool(LogMetadata)> _filter {[](LogMetadata) -> bool { return true; }};
};

} // namespace se_oss

#endif // SE_SE_OSS_LOG_SINK_FILTEREDSINK_H