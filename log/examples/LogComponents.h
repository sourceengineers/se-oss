/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SE_LOGCOMPONENTS_H
#define SE_LOGCOMPONENTS_H

#include <cstdint>

enum class LogComponents : uint8_t
{
    CELLULAR = 0,
    WUFF = 1,
    STORAGE = 2,
};

constexpr uint8_t toUint(LogComponents component) { return static_cast<uint8_t>(component); }
constexpr const char* toString(LogComponents component)
{
    switch (component) {
        case LogComponents::CELLULAR: return "cell";
        case LogComponents::WUFF: return "wuff";
        case LogComponents::STORAGE: return "storage";
    }
    return "";
}

enum class LogSinks : uint8_t
{
    SHELL = 0,
    FILE = 1,
};

constexpr uint8_t toUint(LogSinks sink) { return static_cast<uint8_t>(sink); }
constexpr const char* toString(LogSinks sink)
{
    switch (sink) {
        case LogSinks::SHELL: return "shell";
        case LogSinks::FILE: return "file";
    }
    return "";
}

#endif  // SE_LOGCOMPONENTS_H
