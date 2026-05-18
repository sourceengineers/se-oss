/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "UserLogConf.h"
#include "buffer/IBuffer.h"
#include "buffer/ImmediateBuffer.h"

#include <type_traits>

namespace se_oss {

static_assert(std::is_base_of<IBuffer, log_conf::Buffer>::value, "log_conf::Buffer must implement IBuffer");

namespace log_detail {
template<typename T>
struct is_immediate_buffer : std::false_type
{
};

template<std::size_t N>
struct is_immediate_buffer<ImmediateBuffer<N>> : std::true_type
{
};
}

}  // namespace se_oss
