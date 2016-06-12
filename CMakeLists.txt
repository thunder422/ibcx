# vim:ts=4:sw=4:et:sts=4:
#
# Copyright 2016 Thunder422.  All rights reserved.
# Distributed under GNU General Public License Version 3
# (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)

project(ibc)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -pedantic-errors -std=c++11"
)
if (WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__NO_INLINE__")
endif ()

message(STATUS "Checking for working C++11 compiler: ${CMAKE_CXX_COMPILER}")
try_compile(CPP11_GOOD
    ${PROJECT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/test/cpp11.cpp
    OUTPUT_VARIABLE OUTPUT
)
if (${CPP11_GOOD})
    message(STATUS "Checking for working C++11 compiler: ${CMAKE_CXX_COMPILER} -- works")
else ()
    message(FATAL_ERROR
        "The C++ compiler is not able to compile a simple C++11 test program.\n"
        "It fails with the following output:\n"
        "${OUTPUT}"
    )
endif ()

if (PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
    message(FATAL_ERROR
        "\nbuilding in the source directory not allowed"
        "\nplease create a separate build directory"
    )
endif ()

if (NOT $ENV{CMAKE_BUILD_TYPE} STREQUAL "")
    if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE $ENV{CMAKE_BUILD_TYPE})
    endif ()
endif ()
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif ()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

if (IBC_COVERAGE)
    if (NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "IBC_COVERAGE is only supported with Debug build type")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
        set(GCOV_LIB gcov)
        message(STATUS "Coverage: Activated")
    endif ()
endif ()

include_directories(basic common compiler program)

set(IBC_SOURCES
    basic/code.cpp
    basic/commandcode.cpp
    basic/constnum.cpp
    basic/end.cpp
    basic/print.cpp
    common/cistring.h
    common/compileerror.h
    common/datatype.h
    common/dictionary.cpp
    common/executer.cpp
    common/recreator.cpp
    common/runerror.h
    common/wordtype.h
    compiler/commandcompiler.cpp
    compiler/compiler.cpp
    compiler/constnumcompiler.cpp
    compiler/expressioncompiler.cpp
    program/programcode.cpp
    program/programreader.cpp
    program/programunit.cpp
    program/programword.h
)

add_library(ibc SHARED
    ${IBC_SOURCES}
)

add_executable(ibc-bin
    ibc/main.cpp
)
target_link_libraries(ibc-bin ibc gcov)
set_target_properties(ibc-bin PROPERTIES OUTPUT_NAME ibc)

enable_testing()

add_subdirectory(ibc)
add_subdirectory(test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

function(add_unittest name)
    if (${ARGC} EQUAL 2)
        set(extra ${ARGV1})
    endif ()
    add_executable(${name}_unittests
        test/${name}_unittests.cpp
        ${extra}
        ${CATCH_SOURCES}
    )
    target_link_libraries(${name}_unittests ibc ${GCOV_LIB})
    add_test(${name}_unittests ${name}_unittests)
endfunction(add_unittest)

add_unittest(constnum test/support.h)
add_unittest(expression test/support.h)
add_unittest(command)
add_unittest(cistring)
add_unittest(print)
