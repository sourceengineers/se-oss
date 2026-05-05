/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/sink/ConsoleWriter.h"

#include <cstdint>
#include <cstdio>

namespace se_oss {

ConsoleWriter::ConsoleWriter(bool hexOutput) : _hexOutput {hexOutput} { }

void ConsoleWriter::write(const void* data, size_t length)
{
    if (!_hexOutput) {
        std::fwrite(data, sizeof(char), length, stdout);
    } else {
        auto byteData = static_cast<const uint8_t*>(data);
        for (size_t i = 0; i < length; ++i) {
            std::printf("%02X", byteData[i]);
        }
        std::putchar('\n');
    }
}

}  // namespace se_oss
