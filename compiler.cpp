/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>
#include <string>

#include "compiler.h"
#include "programunit.h"


Compiler::Compiler(const std::string &line, ProgramUnit &program) :
    iss {line},
    program {program}
{
}

ci_string Compiler::getKeyword()
{
    ci_string keyword;
    iss >> std::ws;
    while (isalpha(peekNextChar())) {
        keyword += getNextChar();
    }
    iss >> std::ws;
    return keyword;
}

char Compiler::peekNextChar()
{
    return iss.peek();
}

char Compiler::getNextChar()
{
    return iss.get();
}

char Compiler::getColumn()
{
    return iss.tellg();
}

void Compiler::addInstruction(Code &code)
{
    code_line.emplace_back(code);
}

DataType Compiler::addConstNumInstruction(bool floating_point, const std::string &number)
{
    auto const_num_info = program.constNumDictionary().add(floating_point, number);
    code_line.emplace_back(const_num_info.code_value);
    code_line.emplace_back(const_num_info.operand);
    return const_num_info.data_type;
}

ProgramCode &&Compiler::getCodeLine()
{
    return std::move(code_line);
}
