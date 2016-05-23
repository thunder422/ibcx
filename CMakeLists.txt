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

set(IBC_SOURCES
    code.cpp
    commandcode.cpp
    commandparser.cpp
    constnumparser.cpp
    dictionary.cpp
    datatype.h
    expressionparser.cpp
    parseerror.h
    print.cpp
    programcode.cpp
    programunit.cpp
    programword.h
)

enable_testing()

add_subdirectory(test)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(constnum_unittests
    test/constnum_unittests.cpp
    test/support.h
    ${IBC_SOURCES}
    ${CATCH_SOURCES}
)
target_link_libraries(constnum_unittests ${GCOV_LIB})
add_test(ConstantNumber_UnitTests constnum_unittests)

add_executable(expression_unittests
    test/expression_unittests.cpp
    test/support.h
    ${IBC_SOURCES}
    ${CATCH_SOURCES}
)
target_link_libraries(expression_unittests ${GCOV_LIB})
add_test(Expression_UnitTests expression_unittests)

add_executable(command_unittests
    test/command_unittests.cpp
    ${IBC_SOURCES}
    ${CATCH_SOURCES}
)
target_link_libraries(command_unittests ${GCOV_LIB})
add_test(Command_UnitTests command_unittests)
