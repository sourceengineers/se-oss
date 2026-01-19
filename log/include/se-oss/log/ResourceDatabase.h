/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <utility>

namespace se_oss {

/**
 * Retrieves the unique resource ID for a type.
 *
 * This function is intended to be specialized for specific types to return
 * a unique identifier, often used for string interning or resource management.
 *
 * @tparam T The type to look up.
 * @return The resource ID.
 */
template <typename T>
extern uint32_t getResourceId();

/**
 * Meta-function to represent a resource identifier.
 * @tparam chars The characters of the string resource.
 */
template <char...>
struct ResourceIdentifier {};

/**
 * Builds a ResourceIdentifier type from a string literal type.
 *
 * @tparam String A type with a static `characters` member array.
 * @tparam indices Index sequence for the characters.
 * @return Use decltype to get the ResourceIdentifier type.
 */
template <typename String, std::size_t... indices>
decltype(auto) buildResourceIdentifier(std::index_sequence<indices...>) {
    return ResourceIdentifier<String().characters[indices]...>();
}


} // namespace se_oss
