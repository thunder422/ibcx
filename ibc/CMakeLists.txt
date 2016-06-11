# vim:ts=4:sw=4:et:sts=4:
#
# Copyright 2016 Thunder422.  All rights reserved.
# Distributed under GNU General Public License Version 3
# (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)

add_executable(ibc-bin
    main.cpp
)
target_link_libraries(ibc-bin ibc gcov)
set_target_properties(ibc-bin PROPERTIES OUTPUT_NAME ibc)

add_test(NAME ibc_usage_test
    COMMAND "${CMAKE_COMMAND}"
        -D "TEST_NAME=usage"
        -D "TEST_PROGRAM=$<TARGET_FILE:ibc-bin>"
        -D "TEST_ARGS="
        -D "TEST_EXPECT=1"
        -D "SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/test/runTest.cmake"
)
