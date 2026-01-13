# Copyright (c) 2026 Source Engineers GmbH
# SPDX-License-Identifier: MIT

find_program(GCOVR gcovr)

function(se_coverage)
    add_custom_target(
            se_oss-coverage
            COMMAND mkdir -p ${CMAKE_BINARY_DIR}/report/coverage
            COMMAND ${GCOVR} --xml-pretty --exclude-unreachable-branches --print-summary -o ${CMAKE_BINARY_DIR}/report/se-oss-test-coverage-report.xml --html --html-theme github.dark-green --html-details ${CMAKE_BINARY_DIR}/report/coverage/index.html --root ${CMAKE_SOURCE_DIR} -e /.*/build/ -e /.*/test/ -e /usr* -e .*/lib/
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Create coverage report"
            VERBATIM
    )

    install(
            DIRECTORY ${CMAKE_BINARY_DIR}/report
            DESTINATION .
            COMPONENT tests
    )
endfunction()
