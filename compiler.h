/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef COMPILER_H
#define COMPILER_H

#include <iosfwd>
#include <string>


class Code;
class ProgramCode;
class ProgramUnit;

class Compiler {
public:
    Compiler(std::istream &is, ProgramCode &code_line, ProgramUnit &program);

    std::string getKeyword() const;
    char peekNextChar() const;
    char getNextChar() const;
    char getColumn() const;

    void addInstruction(Code &code) const;
    void addConstNumInstruction(Code &code, const std::string &number) const;

private:
    std::istream &is;
    ProgramCode &code_line;
    ProgramUnit &program;
};


#endif  // COMPILER_H
