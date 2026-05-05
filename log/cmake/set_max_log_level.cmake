# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

function(se_set_max_log_level)
    set(prefix SE)
    set(singleValues TARGET MAX_LEVEL)
    set(multiValues)

    include(CMakeParseArguments)
    cmake_parse_arguments(
            ${prefix}
            "${flags}"
            "${singleValues}"
            "${multiValues}"
            ${ARGN}
    )

    _se_to_log_level(${SE_MAX_LEVEL} level)

    target_compile_definitions(
            ${SE_TARGET}
            PUBLIC SE_LOG_MAX_LOG_LEVEL=${level}
    )
endfunction()

function(_se_to_log_level level_string level_int)
    string(TOUPPER ${level_string} level_upper)

    if (${level_upper} STREQUAL "TRACE")
        set(${level_int} 0 PARENT_SCOPE)
    elseif (${level_upper} STREQUAL "DEBUG")
        set(${level_int} 1 PARENT_SCOPE)
    elseif (${level_upper} STREQUAL "INFO")
        set(${level_int} 2 PARENT_SCOPE)
    elseif (${level_upper} STREQUAL "WARN")
        set(${level_int} 3 PARENT_SCOPE)
    elseif (${level_upper} STREQUAL "ERROR")
        set(${level_int} 4 PARENT_SCOPE)
    elseif (${level_upper} STREQUAL "FATAL")
        set(${level_int} 5 PARENT_SCOPE)
    elseif (${level_upper} STREQUAL "OFF")
        set(${level_int} 255 PARENT_SCOPE)
    else ()
        message(FATAL_ERROR "Log level ${level_string} does not match any of: TRACE, DEBUG, INFO, WARN, ERROR, FATAL or OFF.")
    endif ()
endfunction()
