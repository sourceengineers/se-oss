# Copyright (c) 2025 Source Engineers GmbH, Switzerland
# Licensed under the MIT License, see LICENSE.MIT in the se-oss project root for full terms.
# SPDX-License-Identifier: MIT

function(se_create_resource_database)
    set(prefix SE_LOG_DB)
    set(singleValues TARGET)
    set(multiValues LIBRARIES)

    include(CMakeParseArguments)
    cmake_parse_arguments(
            ${prefix}
            "${flags}"
            "${singleValues}"
            "${multiValues}"
            ${ARGN}
    )

    find_package(Python3)

    set(RESOURCE_SCRIPT_FILE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tools/create_resource_database.py)
    set(TEMPLATE_SOURCE_FILE ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/ResourceDatabase.cpp.in)
    set(GENERATED_SOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/SeResourceDatabase.cpp)
    set(GENERATED_DATABASE_FILE ${CMAKE_CURRENT_BINARY_DIR}/se_log_resource_database.json)

    # Object files to scan for log resources. Log statements compiled into a static library are not
    # part of $<TARGET_OBJECTS:TARGET>, so such libraries have to be named explicitly via LIBRARIES.
    # Without that their format strings never receive an ID and the link fails with undefined
    # references to se_oss::getResourceId<>.
    set(RESOURCE_OBJECTS $<TARGET_OBJECTS:${SE_LOG_DB_TARGET}>)
    foreach (LIBRARY ${SE_LOG_DB_LIBRARIES})
        list(APPEND RESOURCE_OBJECTS $<TARGET_OBJECTS:${LIBRARY}>)
    endforeach ()

    separate_arguments(COMPILER_FLAGS NATIVE_COMMAND ${CMAKE_CXX_FLAGS})

    # CMAKE_CXX_FLAGS does not carry the -std option, so the generated database would otherwise be
    # compiled with the compiler default instead of the standard the target is built with. The flag
    # is placed before CMAKE_CXX_FLAGS so that an explicit -std in there still wins.
    get_property(TARGET_CPP_STANDARD TARGET ${SE_LOG_DB_TARGET} PROPERTY CXX_STANDARD)
    if (TARGET_CPP_STANDARD)
        set(STANDARD_FLAG -std=c++${TARGET_CPP_STANDARD})
    endif ()

    add_custom_command(
            COMMENT "Assigning IDs to log messages"
            TARGET ${SE_LOG_DB_TARGET}
            PRE_LINK
            COMMAND echo "[se-log] Creating database for log messages"
            COMMAND ${CMAKE_NM} -uC ${RESOURCE_OBJECTS} | ${Python3_EXECUTABLE} ${RESOURCE_SCRIPT_FILE} ${TEMPLATE_SOURCE_FILE} ${GENERATED_SOURCE_FILE} ${GENERATED_DATABASE_FILE}
            # In order to prevent a dependency cycle we compile the file manually and do not add it to BYPRODUCTS
            COMMAND ${CMAKE_CXX_COMPILER} -I ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../include ${STANDARD_FLAG} ${COMPILER_FLAGS} -c ${GENERATED_SOURCE_FILE} -o ${GENERATED_SOURCE_FILE}.o
            VERBATIM
            COMMAND_EXPAND_LISTS
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    target_link_options(
            ${SE_LOG_DB_TARGET}
            PUBLIC
            ${GENERATED_SOURCE_FILE}.o
    )
    
    install(FILES ${GENERATED_DATABASE_FILE} DESTINATION firmware COMPONENT firmware)
endfunction()
