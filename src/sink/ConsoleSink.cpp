/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/sink/ConsoleSink.h"

#include <cstdio>

namespace se_oss {

ConsoleSink::ConsoleSink(bool hexOutput) : _hexOutput {hexOutput}
{

}

void ConsoleSink::write(LogMetadata metadata, const void* data, size_t length)
{
    if (metadata.logLevel < _level) {
        return;
    }

    if (!_hexOutput) {
        printf("%s\n", static_cast<const char*>(data));
    } else {
        auto byteData = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < length; ++i) {
            printf("%02X", byteData[i]);
        }
        putchar('\n');
    }
}

void ConsoleSink::setLogLevel(LogLevel level)
{
    _level = level;
}

void ConsoleSink::setComponentLogLevel(LogLevel level, uint8_t componentId)
{
    // todo: component filter
    _level = level;
}

} // namespace ${SE_NAMESPACE}