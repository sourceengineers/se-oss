// Copyright (c) 2025 Source Engineers GmbH
// SPDX-License-Identifier: MIT

#include "se-oss/log/sink/NullSink.h"

#include <gtest/gtest.h>

using namespace se_oss;

TEST(NullSink, WriteDoesNotCrash)
{
    NullSink sink;
    uint8_t data[] = {1, 2, 3};
    EXPECT_NO_THROW(sink.write(data, sizeof(data)));
}

TEST(NullSink, FlushDoesNotCrash)
{
    NullSink sink;
    EXPECT_NO_THROW(sink.flush());
}
