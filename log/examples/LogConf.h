/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

enum class MyLogContext : uint8_t
{
    CELLULAR = 0,
    WUFF = 1,
    STORAGE = 2,
};

constexpr uint8_t toUint(MyLogContext component) { return static_cast<uint8_t>(component); }
constexpr const char* toString(MyLogContext component)
{
    switch (component) {
        case MyLogContext::CELLULAR: return "cell";
        case MyLogContext::WUFF: return "wuff";
        case MyLogContext::STORAGE: return "storage";
    }
    return "";
}

enum class MyLogSink : uint8_t
{
    SHELL = 0,
    FILE = 1,
};

constexpr uint8_t toUint(MyLogSink sink) { return static_cast<uint8_t>(sink); }
constexpr const char* toString(MyLogSink sink)
{
    switch (sink) {
        case MyLogSink::SHELL: return "shell";
        case MyLogSink::FILE: return "file";
    }
    return "";
}
