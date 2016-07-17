/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>
#include <string>

#include "compiler.h"
#include "expressioncompiler.h"
#include "programunit.h"


Compiler::Compiler(const std::string &line, ProgramUnit &program) :
    iss {line},
    program {program}
{
}

DataType Compiler::compileExpression(DataType expected_data_type)
{
    return ExpressionCompiler{*this}(expected_data_type);
}

ci_string Compiler::getKeyword()
{
    ci_string keyword;
    skipWhiteSpace();
    while (isalpha(peekNextChar())) {
        keyword += getNextChar();
    }
    skipWhiteSpace();
    return keyword;
}

char Compiler::peekNextChar()
{
    if (!peek_char) {
        column = iss.tellg();
        peek_char = iss.peek();
    }
    return peek_char;
}

char Compiler::getNextChar()
{
    peek_char = 0;
    return iss.get();
}

void Compiler::skipWhiteSpace()
{
    if (peek_char != EOF) {
        peek_char = 0;
        iss >> std::ws;
    }
}

unsigned Compiler::getColumn() noexcept
{
    return column;
}

void Compiler::addInstruction(Code &code)
{
    last_constant_data_type = DataType::Null;
    code_line.emplace_back(code);
}

DataType Compiler::addNumConstInstruction(bool floating_point, const std::string &number)
{
    auto const_num_info = program.constNumDictionary().add(floating_point, number);
    last_constant_data_type = const_num_info.data_type;
    code_line.emplace_back(const_num_info.code_value);
    code_line.emplace_back(const_num_info.operand);
    return const_num_info.data_type;
}

void Compiler::convertToDouble()
{
    extern Code const_dbl_code;
    if (last_constant_data_type == DataType::Integer) {
        auto last_constant_offset = code_line.size() - 2;
        code_line[last_constant_offset] = ProgramWord {const_dbl_code};
    }
}

ProgramCode &&Compiler::getCodeLine()
{
    return std::move(code_line);
}
