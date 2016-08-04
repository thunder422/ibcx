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
    program {program},
    equality_codes {}
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

class ComparisonOperatorCodes {
public:
    ComparisonOperatorCodes(Compiler &compiler) : compiler {compiler} { }
    OperatorCodes *operator()();

private:
    OperatorCodes *codesWithNextPeekChar();
    OperatorCodes *relationCodes(const ComparisonOperator &comparison_operator);

    Compiler &compiler;
    std::string keyword;
    OperatorCodes *one_char_codes;
};

OperatorCodes *Compiler::getComparisonOperatorCodes(Precedence::Level precedence)
{
    if (precedence == Precedence::Equality) {
        return savedEqualityOperatorCodes();
    } else {
        skipWhiteSpace();
        return ComparisonOperatorCodes(*this)();
    }
}

OperatorCodes *Compiler::savedEqualityOperatorCodes()
{
    auto codes = equality_codes;
    equality_codes = nullptr;
    return codes;
}

OperatorCodes *ComparisonOperatorCodes::operator()()
{
    if ((one_char_codes = codesWithNextPeekChar())) {
        if (auto two_char_codes = codesWithNextPeekChar()) {
            return two_char_codes;
        }
        return one_char_codes;
    }
    return nullptr;
}

OperatorCodes *ComparisonOperatorCodes::codesWithNextPeekChar()
{
    keyword += compiler.peekNextChar();
    auto comparison_operator = Precedence::comparisonOperator(keyword);
    if (comparison_operator.codes) {
        compiler.getNextChar();
        compiler.skipWhiteSpace();
        return relationCodes(comparison_operator);
    }
    return nullptr;
}

OperatorCodes *ComparisonOperatorCodes::relationCodes(const ComparisonOperator &comparison_operator)
{
    if (comparison_operator.precedence == Precedence::Relation) {
        return comparison_operator.codes;
    } else {
        compiler.setEqualityCodes(comparison_operator.codes);
        one_char_codes = nullptr;
        return nullptr;
    }
}

void Compiler::setEqualityCodes(OperatorCodes *codes)
{
    equality_codes = codes;
}

ci_string Compiler::getKeyword()
{
    skipWhiteSpace();
    word = getAlphaOnlyWord();
    return word;
}

ci_string Compiler::getAlphaOnlyWord()
{
    ci_string keyword;
    while (isalpha(peekNextChar())) {
        keyword += getNextChar();
    }
    skipWhiteSpace();
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
        while (isspace(peekNextChar())) {
            getNextChar();
        }
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
