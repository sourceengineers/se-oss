/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#include "se-oss/log/buffer/NoBuffer.h"

#include <cstring>
#include <gtest/gtest.h>

using namespace se_oss;

class AtomicBufferTest : public ::testing::Test { };

TEST_F(AtomicBufferTest, InitialState) {
    NoBuffer<128> buffer;
    EXPECT_EQ(buffer.capacity(), 0);
    EXPECT_EQ(buffer.size(), 0);
    EXPECT_EQ(buffer.free(), 0);
}
