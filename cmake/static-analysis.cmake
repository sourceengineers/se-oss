# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

find_program(CLANG_TIDY clang-tidy)
find_program(CLANG_FORMAT clang-format)
find_program(CPP_CHECK cppcheck)

function(se_static_analysis)
    # parse input arguments
    set(prefix CHECK)
    set(singleValues TARGET)
    set(multiValues)

    include(CMakeParseArguments)
    cmake_parse_arguments(${prefix}
            "${flags}"
            "${singleValues}"
            "${multiValues}"
            ${ARGN})

    # Collect target sources
    get_property(CHECK_SOURCES TARGET ${CHECK_TARGET} PROPERTY SOURCES)
    get_property(TARGET_ISOURCES TARGET ${CHECK_TARGET} PROPERTY INTERFACE_SOURCES)
    list(APPEND CHECK_SOURCES ${TARGET_ISOURCES})

    get_property(CHECK_HEADERS TARGET ${CHECK_TARGET} PROPERTY HEADER_SET)

    # Collect include directories
    get_property(TARGET_DIRS TARGET ${CHECK_TARGET} PROPERTY INCLUDE_DIRECTORIES)
    get_property(TARGET_IDIRS TARGET ${CHECK_TARGET} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND TARGET_DIRS ${TARGET_IDIRS})

    get_property(TARGET_LINK_LIBS TARGET ${CHECK_TARGET} PROPERTY LINK_LIBRARIES)
    foreach(LIBRARY ${TARGET_LINK_LIBS})
        get_property(TARGET_LINK_DIR TARGET ${LIBRARY} PROPERTY INTERFACE_INCLUDE_DIRECTORIES)
        list(APPEND TARGET_DIRS ${TARGET_LINK_DIR})
    endforeach()

    # Collect compile definitions
    get_property(TARGET_DEFINITIONS TARGET ${CHECK_TARGET} PROPERTY COMPILE_DEFINITIONS)
    get_property(TARGET_IDEFINITIONS TARGET ${CHECK_TARGET} PROPERTY ICOMPILE_DEFINITIONS)
    list(APPEND TARGET_DEFINITIONS ${TARGET_IDEFINITIONS})

    # Get c++ version
    get_property(TARGET_CPP_STANDARD TARGET ${CHECK_TARGET} PROPERTY CXX_STANDARD)

    foreach (DIR ${TARGET_DIRS})
        # note: generator expressions in DIR could lead to an empty variable at
        #       compile-time, so we need to add -I conditionally
        list(APPEND TARGET_INC_DIRS "$<$<BOOL:${DIR}>:-I${DIR}>")
    endforeach ()
    list(TRANSFORM TARGET_DEFINITIONS PREPEND "-D")

    # clang-tidy target
    if (EXISTS ${CLANG_TIDY})
        if ("${CHECK_HEADERS}" STREQUAL "")
            add_custom_target(
                    ${CHECK_TARGET}-clang-tidy
                    # run check on sources
                    COMMAND ${CLANG_TIDY} ${CHECK_SOURCES} -config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -- -std=c++${TARGET_CPP_STANDARD} ${TARGET_INC_DIRS} ${TARGET_DEFINITIONS}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    VERBATIM
            )
        else ()
            add_custom_target(
                    ${CHECK_TARGET}-clang-tidy
                    # run check on sources
                    COMMAND ${CLANG_TIDY} ${CHECK_SOURCES} -config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -- -std=c++${TARGET_CPP_STANDARD} ${TARGET_INC_DIRS} ${TARGET_DEFINITIONS}
                    # run check on headers
                    COMMAND ${CLANG_TIDY} ${CHECK_HEADERS} -config-file=${CMAKE_SOURCE_DIR}/.clang-tidy -- -x c++-header -std=c++${TARGET_CPP_STANDARD} ${TARGET_INC_DIRS} ${TARGET_DEFINITIONS}
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    VERBATIM
            )
        endif ()
    endif ()

    # cppcheck target
    if (EXISTS ${CPP_CHECK})
        add_custom_target(
                ${CHECK_TARGET}-cppcheck
                COMMAND ${CPP_CHECK}
                --enable=all
                --suppress=missingIncludeSystem
                --suppress=missingInclude:*
                --suppress=unusedFunction:*
                --suppress=unmatchedSuppression
                --suppress=unmatchedSuppression:{}
                --suppress=unusedStructMember
                -ibuild  # ignore sources in build directory (e.g. _deps)
                --inconclusive
                --force
                --inline-suppr
                --std=c++${TARGET_CPP_STANDARD}
                --language=c++
                --error-exitcode=1  # exit code if errors are found (makes CI pipeline fail)
                --template="{file}:{line}:{id}:{severity}: {message}"
                ${TARGET_INC_DIRS}
                ${TARGET_DEFINITIONS}
                ${CHECK_SOURCES}
                ${CHECK_HEADERS}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif ()

    # clang-format
    if (EXISTS ${CLANG_FORMAT})
        add_custom_target(
                ${CHECK_TARGET}-clang-format
                # run check on sources
                COMMAND ${CLANG_FORMAT}
                -style=file
                --dry-run
                --Werror
                ${CHECK_SOURCES}
                ${CHECK_HEADERS}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )

        add_custom_target(
                ${CHECK_TARGET}-clang-format-apply-fixes
                # run check on sources
                COMMAND ${CLANG_FORMAT}
                -style=file
                -i
                ${CHECK_SOURCES}
                ${CHECK_HEADERS}
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        )
    endif ()
endfunction()