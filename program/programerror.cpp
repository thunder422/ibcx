/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "programerror.h"
#include "runerror.h"


ProgramError::ProgramError(const RunError &run_error) :
    runtime_error {run_error.what()}
{
}

ProgramError::ProgramError(const RunError &run_error, unsigned line_number,
        const std::string &program_line) :
    runtime_error {run_error.what()},
    line_number {line_number},
    column {program_line.find_first_of(StartErrorMarker)},
    length {1},
    line {program_line}
{
    line.erase(column, 1);
}
