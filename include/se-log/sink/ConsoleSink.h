/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_LOG_PRINTFSINK_H
#define SE_LOG_PRINTFSINK_H

#include "ILogSink.h"
#include "se-log/ILogFilter.h"

namespace se_oss {

class ConsoleSink final :
    public ILogSink,
    public ILogFilter
{
public:
    explicit ConsoleSink(bool hexOutput = false);
    ~ConsoleSink() override = default;
    ConsoleSink(const ConsoleSink&) = delete;
    ConsoleSink(ConsoleSink&&) = delete;
    ConsoleSink& operator=(const ConsoleSink&) = delete;
    ConsoleSink& operator=(ConsoleSink&&) = delete;

    void write(LogMetadata metadata, const void* data, size_t length) override;
    void flush() override {}

    void setLogLevel(LogLevel level) override;
    void setComponentLogLevel(LogLevel level, uint8_t componentId) override;

private:
    bool _hexOutput;
    LogLevel _level {LogLevel::TRACE};
};

} // namespace se

#endif // SE_LOG_PRINTFSINK_H
