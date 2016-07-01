/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_PROGRAMERROR_H
#define IBC_PROGRAMERROR_H

#include <iosfwd>
#include <stdexcept>
#include <string>


constexpr char StartErrorMarker = '\02';

struct CompileError;
struct RunError;

struct ProgramError : public std::runtime_error {
    ProgramError(const CompileError &error, unsigned line_number, const std::string &program_line);
    ProgramError(const RunError &run_error);
    ProgramError(const RunError &error, unsigned line_number, const std::string &program_line);

    void output(std::ostream &os) const;

    unsigned line_number;
    size_t column;
    unsigned length;
    std::string line;

private:
    const char *typeString() const;

    enum Type {Compile, Run} type;
};


#endif  // IBC_PROGRAMERROR_H
