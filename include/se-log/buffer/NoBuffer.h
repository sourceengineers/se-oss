/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_BUFFER_NOBUFFER_H
#define SE_SE_LOG_BUFFER_NOBUFFER_H


#include "ILogBuffer.h"

namespace se {
class NoBuffer final : ILogBuffer
{
public:
    NoBuffer() = default;
    ~NoBuffer() override = default;
    NoBuffer(const NoBuffer&) = delete;
    NoBuffer(NoBuffer&&) = delete;
    NoBuffer& operator=(const NoBuffer&) = delete;
    NoBuffer& operator=(NoBuffer&&) = delete;



private:
};

} // namespace se

#endif // SE_SE_LOG_BUFFER_NOBUFFER_H