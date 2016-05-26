/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>
#include <string>

#include "compiler.h"
#include "programcode.h"
#include "programunit.h"


Compiler::Compiler(std::istream &is, ProgramCode &code_line, ProgramUnit &program) :
    is {is},
    code_line {code_line},
    program {program}
{
}

std::string Compiler::getKeyword() const
{
    std::string keyword;
    is >> std::ws;
    while (isalpha(peekNextChar())) {
        keyword += getNextChar();
    }
    is >> std::ws;
    return keyword;
}

char Compiler::peekNextChar() const
{
    return is.peek();
}

char Compiler::getNextChar() const
{
    return is.get();
}

char Compiler::getColumn() const
{
    return is.tellg();
}

void Compiler::addInstruction(Code &code) const
{
    code_line.emplace_back(code);
}

void Compiler::addConstNumInstruction(Code &code, const std::string &number) const
{
    code_line.emplace_back(code);
    code_line.emplace_back(program.constNumDictionary().add(number));
}
