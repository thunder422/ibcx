/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef COMPILER_H
#define COMPILER_H

#include <iosfwd>


class ProgramCode;
class ProgramUnit;

class Compiler {
public:
    Compiler(std::istream &is, ProgramCode &code_line, ProgramUnit &program);

    std::istream &is;
    ProgramCode &code_line;
    ProgramUnit &program;
};


#endif  // COMPILER_H
