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
#include "operators.h"
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

OperatorCodes *Compiler::getSymbolOperatorCodes(Precedence::Level precedence)
{
    skipWhiteSpace();
    if (auto codes = Precedence::operatorCodes(precedence, peekNextChar())) {
        getNextChar();
        skipWhiteSpace();
        return codes;
    }
    return nullptr;
}

OperatorCodes *Compiler::getWordOperatorCodes(Precedence::Level precedence)
{
    if (auto codes = Precedence::operatorCodes(precedence, getKeyword())) {
        clearWord();
        return codes;
    }
    return nullptr;
}

class CheckForComparisonOperatorCodes {
public:
    CheckForComparisonOperatorCodes(Compiler &compiler) : compiler {compiler} { }
    OperatorCodes *operator()();

private:
    OperatorCodes *codesWithNextPeekChar();

    Compiler &compiler;
    std::string keyword;
};

OperatorCodes *Compiler::getComparisonOperatorCodes()
{
    skipWhiteSpace();
    if (auto codes = CheckForComparisonOperatorCodes(*this)()) {
        skipWhiteSpace();
        return codes;
    }
    return nullptr;
}

OperatorCodes *CheckForComparisonOperatorCodes::operator()()
{
    if (auto one_char_codes = codesWithNextPeekChar()) {
        if (auto two_char_codes = codesWithNextPeekChar()) {
            return two_char_codes;
        }
        return one_char_codes;
    }
    return nullptr;
}

OperatorCodes *CheckForComparisonOperatorCodes::codesWithNextPeekChar()
{
    keyword += compiler.peekNextChar();
    if (auto codes = Precedence::comparisonOperatorData(keyword)) {
        compiler.getNextChar();
        return codes;
    }
    return nullptr;
}

ci_string Compiler::getKeyword()
{
    skipWhiteSpace();
    word = getAlphaOnlyWord();
    skipWhiteSpace();
    return word;
}

ci_string Compiler::getAlphaOnlyWord()
{
    ci_string keyword;
    while (isalpha(peekNextChar())) {
        keyword += getNextChar();
    }
    return keyword;
}

void Compiler::clearWord()
{
    word.clear();
}

char Compiler::peekNextChar()
{
    if (!word.empty()) {
        return word.front();
    } else if (!peek_char) {
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
    last_operand_was_constant = false;
    code_line.emplace_back(code);
}

DataType Compiler::addNumConstInstruction(bool floating_point, const std::string &number)
{
    auto const_num_info = program.constNumDictionary().add(floating_point, number);
    last_operand_was_constant = true;
    code_line.emplace_back(const_num_info.code_value);
    code_line.emplace_back(const_num_info.operand);
    return const_num_info.data_type;
}

void Compiler::convertToDouble(DataType operand_data_type)
{
    extern Code const_dbl_code;
    extern Code cvtdbl_code;

    if (operand_data_type == DataType::Integer) {
        if (last_operand_was_constant) {
            auto last_constant_offset = code_line.size() - 2;
            code_line[last_constant_offset] = ProgramWord {const_dbl_code};
        } else {
            addInstruction(cvtdbl_code);
        }
    }
}

ProgramCode &&Compiler::getCodeLine()
{
    return std::move(code_line);
}
