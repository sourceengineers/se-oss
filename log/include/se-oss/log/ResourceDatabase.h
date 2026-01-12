/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <utility>

namespace se_oss {

template <typename T>
extern uint32_t getResourceId();

template <char...>
struct ResourceIdentifier {};

template <typename String, std::size_t... indices>
decltype(auto) buildResourceIdentifier(std::index_sequence<indices...>) {
    return ResourceIdentifier<String().characters[indices]...>();
}


} // namespace se
