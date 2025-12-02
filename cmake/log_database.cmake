function(se_create_log_database)
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

    set(GENERATED_SOURCE ${CMAKE_CURRENT_BINARY_DIR}/SeLogStringDatabase.cpp)
    separate_arguments(COMPILER_FLAGS NATIVE_COMMAND ${CMAKE_CXX_FLAGS})

    add_custom_command(
            COMMENT "Assigning IDs to log messages"
            TARGET ${SE_LOG_DB_TARGET}
            PRE_LINK
            COMMAND echo "[se-log] Creating database for log messages"
            COMMAND ${CMAKE_NM} -uC $<TARGET_OBJECTS:${SE_LOG_DB_TARGET}> | python3 ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tools/create_string_database.py ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../tools/StringDatabase.cpp.in ${GENERATED_SOURCE} ${CMAKE_CURRENT_BINARY_DIR}/se_log_string_database.json
            # In order to prevent a dependency cycle we compile the file manually and do not add it to BYPRODUCTS
            COMMAND ${CMAKE_CXX_COMPILER} -I ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/../include ${COMPILER_FLAGS} -c ${GENERATED_SOURCE} -o ${GENERATED_SOURCE}.o
            VERBATIM
            COMMAND_EXPAND_LISTS
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )

    target_link_options(
            ${SE_LOG_DB_TARGET}
            PUBLIC
            ${GENERATED_SOURCE}.o
    )
endfunction()