/*
 * Copyright (c) 2025 Source Engineers GmbH
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>

enum class LogContext : uint8_t
{
    CELLULAR = 0,
    WUFF = 1,
    STORAGE = 2,
};

constexpr uint8_t toUint(LogContext component) { return static_cast<uint8_t>(component); }
constexpr const char* toString(LogContext component)
{
    switch (component) {
        case LogContext::CELLULAR: return "cell";
        case LogContext::WUFF: return "wuff";
        case LogContext::STORAGE: return "storage";
    }
    return "";
}

enum class LogSink : uint8_t
{
    SHELL = 0,
    FILE = 1,
};

constexpr uint8_t toUint(LogSink sink) { return static_cast<uint8_t>(sink); }
constexpr const char* toString(LogSink sink)
{
    switch (sink) {
        case LogSink::SHELL: return "shell";
        case LogSink::FILE: return "file";
    }
    return "";
}
