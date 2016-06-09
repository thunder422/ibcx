/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_COMPILEERROR_H
#define IBC_COMPILEERROR_H

#include <stdexcept>


struct CompileError : public std::runtime_error {
    CompileError(const char *message, int column, size_t length = 1) :
        runtime_error {message}, column {column}, length {length} { }

    int column;
    size_t length;
};


#endif  // IBC_COMPILEERROR_H
