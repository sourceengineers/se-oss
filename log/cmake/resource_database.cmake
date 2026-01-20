# Copyright (c) 2025 Source Engineers GmbH
# SPDX-License-Identifier: MIT

function(se_create_resource_database)
    set(prefix SE_LOG_DB)
    set(singleValues TARGET)
    set(multiValues)

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

    separate_arguments(COMPILER_FLAGS NATIVE_COMMAND ${CMAKE_CXX_FLAGS})

    add_custom_command(
            COMMENT "Assigning IDs to log messages"
            TARGET ${SE_LOG_DB_TARGET}
            PRE_LINK
            COMMAND echo "[se-log] Creating database for log messages"
            COMMAND ${CMAKE_NM} -uC $<TARGET_OBJECTS:${SE_LOG_DB_TARGET}> | ${Python3_EXECUTABLE} ${RESOURCE_SCRIPT_FILE} ${TEMPLATE_SOURCE_FILE} ${GENERATED_SOURCE_FILE} ${GENERATED_DATABASE_FILE}
            # In order to prevent a dependency cycle we compile the file manually and do not add it to BYPRODUCTS
            COMMAND ${CMAKE_CXX_COMPILER} -I ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../include ${COMPILER_FLAGS} -c ${GENERATED_SOURCE_FILE} -o ${GENERATED_SOURCE_FILE}.o
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