/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMERROR_H
#define IBC_PROGRAMERROR_H

#include <stdexcept>
#include <string>


constexpr char StartErrorMarker = '\02';

struct RunError;

struct ProgramError : public std::runtime_error {
    ProgramError(const RunError &run_error);
    ProgramError(const RunError &run_error, unsigned line_number, const std::string &program_line);

    unsigned line_number;
    size_t column;
    unsigned length;
    std::string line;
};


#endif  // IBC_PROGRAMERROR_H
