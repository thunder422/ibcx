# vim:ts=4:sw=4:et:sts=4:
#
# Copyright 2016 Thunder422.  All rights reserved.
# Distributed under GNU General Public License Version 3
# (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)

set(CATCH_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/catch.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/catch_main.cpp
    PARENT_SCOPE
)

if (GCOV_LIB)
    add_custom_target(runibccov ALL
        COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CMAKE_CURRENT_SOURCE_DIR}/runibccov
            ${CMAKE_BINARY_DIR}/runibccov
        DEPENDS runibccov
    )
    set_directory_properties(PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES ${CMAKE_BINARY_DIR}/runibccov
    )
endif ()
