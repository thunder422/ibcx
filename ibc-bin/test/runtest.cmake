# vim:ts=4:sw=4:et:sts=4:
#
# Copyright 2016 Thunder422.  All rights reserved.
# Distributed under GNU General Public License Version 3
# (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)

set(out_file ${TEST_NAME}.out)
set(exp_file ${TEST_NAME}.exp)

execute_process(COMMAND ${TEST_PROGRAM} ${TEST_ARGS}
    WORKING_DIRECTORY ${SOURCE_DIR}
    RESULT_VARIABLE result
    OUTPUT_FILE ${BINARY_DIR}/${out_file}
    ERROR_FILE ${BINARY_DIR}/${out_file}
)

if (NOT ${result} STREQUAL ${TEST_EXPECT})
    message(FATAL_ERROR "Test failed: ${TEST_PROGRAM} returned ${result}, expected ${TEST_EXPECT}")
endif ()

execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files
    ${BINARY_DIR}/${out_file} ${SOURCE_DIR}/${exp_file}
    RESULT_VARIABLE mismatch
)
if (mismatch)
    message(FATAL_ERROR "Test failed: '${out_file}' doesn't match '${exp_file}'")
else ()
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove ${out_file})
endif ()
