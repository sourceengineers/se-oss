# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

find_program(CLANG_TIDY clang-tidy HINTS /usr/bin/clang-tidy)

function(se_clang_tidy)
    # parse input arguments
    set(prefix CLANG_TIDY)
    set(singleValues TARGET)
    set(multiValues)

    include(CMakeParseArguments)
    cmake_parse_arguments(${prefix}
            "${flags}"
            "${singleValues}"
            "${multiValues}"
            ${ARGN})

    # Collect target sources
    get_property(CHECK_FILES TARGET ${CLANG_TIDY_TARGET} PROPERTY SOURCES)
    get_property(TARGET_ISOURCES TARGET ${CLANG_TIDY_TARGET} PROPERTY INTERFACE_SOURCES)
    list(APPEND CHECK_FILES ${TARGET_ISOURCES})

    get_property(CHECK_HEADERS TARGET ${CLANG_TIDY_TARGET} PROPERTY HEADER_SET)

    # Collect include directories
    get_property(TARGET_DIRS TARGET ${CLANG_TIDY_TARGET} PROPERTY INCLUDE_DIRECTORIES)
    get_property(TARGET_IDIRS TARGET ${CLANG_TIDY_TARGET} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND TARGET_DIRS ${TARGET_IDIRS})

    get_property(TARGET_LINK_LIBS TARGET ${CLANG_TIDY_TARGET} PROPERTY LINK_LIBRARIES)
    foreach(LIBRARY ${TARGET_LINK_LIBS})
        get_property(TARGET_LINK_DIR TARGET ${LIBRARY} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
        list(APPEND TARGET_DIRS ${TARGET_LINK_DIR})
    endforeach()

    # Collect compile definitions
    get_property(TARGET_DEFINITIONS TARGET ${CLANG_TIDY_TARGET} PROPERTY COMPILE_DEFINITIONS)
    get_property(TARGET_IDEFINITIONS TARGET ${CLANG_TIDY_TARGET} PROPERTY ICOMPILE_DEFINITIONS)
    list(APPEND TARGET_DEFINITIONS ${TARGET_IDEFINITIONS})

    # Get c++ version
    get_property(TARGET_CPP_STANDARD TARGET ${CLANG_TIDY_TARGET} PROPERTY CXX_STANDARD)

    foreach (DIR ${TARGET_DIRS})
        # note: generator expressions in DIR could lead to an empty variable at
        #       compile-time, so we need to add -I conditionally
        list(APPEND TARGET_INC_DIRS "$<$<BOOL:${DIR}>:-I${DIR}>")
    endforeach ()
    list(TRANSFORM TARGET_DEFINITIONS PREPEND "-D")

    if ("${CHECK_HEADERS}" STREQUAL "")
        add_custom_target(
                ${CLANG_TIDY_TARGET}_clang_tidy
                # run check on sources
                COMMAND ${CLANG_TIDY} ${CHECK_FILES} -config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -- -std=c++${TARGET_CPP_STANDARD} ${TARGET_INC_DIRS} ${TARGET_DEFINITIONS}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                VERBATIM
        )
    else ()
        add_custom_target(
                ${CLANG_TIDY_TARGET}_clang_tidy
                # run check on sources
                COMMAND ${CLANG_TIDY} ${CHECK_FILES} -config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -- -std=c++${TARGET_CPP_STANDARD} ${TARGET_INC_DIRS} ${TARGET_DEFINITIONS}
                # run check on headers
                COMMAND ${CLANG_TIDY} ${CHECK_HEADERS} -config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -- -x c++-header -std=c++${TARGET_CPP_STANDARD} ${TARGET_INC_DIRS} ${TARGET_DEFINITIONS}
                WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                VERBATIM
        )
    endif ()
endfunction()