# vim:ts=4:sw=4:et:sts=4:
#
# Copyright 2016 Thunder422.  All rights reserved.
# Distributed under GNU General Public License Version 3
# (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)

function(add_ibc_test name args expect)
    add_test(NAME ibc_${name}_test
        COMMAND "${CMAKE_COMMAND}"
            -D "TEST_NAME=${name}"
            -D "TEST_PROGRAM=$<TARGET_FILE:ibc-bin>"
            -D "TEST_ARGS=${args}"
            -D "TEST_EXPECT=${expect}"
            -D "SOURCE_DIR=${CMAKE_CURRENT_SOURCE_DIR}/test"
            -D "BINARY_DIR=${CMAKE_CURRENT_BINARY_DIR}"
            -P "${CMAKE_CURRENT_SOURCE_DIR}/test/runtest.cmake"
    )
endfunction(add_ibc_test)

add_ibc_test(usage "" 1)
add_ibc_test(simple simple.bas 0)
add_ibc_test(nofile nofile.bas 1)
add_ibc_test(errors errors.bas 1)
add_ibc_test(recreate "-r;simple.bas" 0)
add_ibc_test(badoption "-q;simple.bas" 1)
add_ibc_test(extraoption "-r;simple.bas;extra" 1)
add_ibc_test(runerror runerror.bas 1)
add_ibc_test(operators "-r;operators.bas" 0)
add_ibc_test(functions "-r;functions.bas" 0)
