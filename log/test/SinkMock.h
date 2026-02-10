/*
 * Copyright (c) 2026 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "se-oss/log/sink/IWriter.h"

#include <gmock/gmock.h>

class SinkMock : public se_oss::IWriter
{
public:
    SinkMock() = default;

    MOCK_METHOD(void, write, (const void*, std::size_t), override);
    MOCK_METHOD(void, flush, (), override);
};
