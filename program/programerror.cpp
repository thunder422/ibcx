/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>

#include "compileerror.h"
#include "programerror.h"
#include "runerror.h"


ProgramError::ProgramError(const CompileError &error, unsigned line_number,
        const std::string &program_line) :
    runtime_error {error.what()},
    line_number {line_number},
    column {error.column},
    length {error.length},
    line {program_line},
    type {Type::Compile}
{
}

ProgramError::ProgramError(const RunError &run_error) :
    runtime_error {run_error.what()},
    type {Type::Run}
{
}

ProgramError::ProgramError(const RunError &run_error, unsigned line_number,
        const std::string &program_line) :
    runtime_error {run_error.what()},
    line_number {line_number},
    column {program_line.find_first_of(StartErrorMarker)},
    length {1},
    line {program_line},
    type {Type::Run}
{
    line.erase(column, 1);
}

void ProgramError::output(std::ostream &os) const
{
    os << typeString() << ' ';
    if (line.empty()) {
        os << "end of program: " << what() << std::endl;
    } else {
        os << "line " << line_number << ':' << column << ": " << what() << std::endl;
        os << "    " << line << std::endl;
        std::string spaces(4 + column, ' ');
        std::string indicator(length, '^');
        os << spaces << indicator << std::endl;
    }
}

const char *ProgramError::typeString() const
{
    return type == Type::Compile ? "error on" : "run error at";
}
