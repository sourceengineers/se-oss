/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_SE_LOG_FORMAT_STRINGLOOKUP_H
#define SE_SE_LOG_FORMAT_STRINGLOOKUP_H

#include <cstdint>
#include <utility>

namespace se_oss {

template <typename T>
extern uint32_t getStringId();

template <char...>
struct TemplateText {};

template <typename String, std::size_t... indices>
decltype(auto) buildStringTypename(std::index_sequence<indices...>) {
    return TemplateText<String().characters[indices]...>();
}


} // namespace se

#endif // SE_SE_LOG_FORMAT_STRINGLOOKUP_H