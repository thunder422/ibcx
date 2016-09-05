/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <iostream>
#include <string>

#include "compileerror.h"
#include "compiler.h"
#include "expressioncompiler.h"
#include "operators.h"
#include "programunit.h"


Compiler::Compiler(const std::string &line, ProgramUnit &program) :
    iss {line},
    program {program}
{
}

void Compiler::compileExpression(DataType expected_data_type)
{
    ExpressionCompiler::create(*this)->compileExpression(expected_data_type);
}

DataType Compiler::compileExpression()
{
    return ExpressionCompiler::create(*this)->compileExpression();
}

DataType Compiler::compileStringConstant()
{
    if (peekNextChar() != '"') {
        return DataType::Null;
    }
    getNextChar();
    while (peekNextChar() != EOF) {
        getNextChar();
        if (peekNextChar() == '"') {
            getNextChar();
            if (peekNextChar() != '"') {
                break;
            }
        }
    }
    return DataType::String;
}

OperatorCodes *Compiler::getSymbolOperatorCodes(Precedence precedence)
{
    skipWhiteSpace();
    if (auto codes = Table::operatorCodes(precedence, peekNextChar())) {
        getNextChar();
        skipWhiteSpace();
        return codes;
    }
    return nullptr;
}

OperatorCodes *Compiler::getWordOperatorCodes(Precedence precedence)
{
    if (auto codes = Table::operatorCodes(precedence, getKeyword())) {
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

OperatorCodes *Compiler::getComparisonOperatorCodes(Precedence precedence)
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
    auto comparison_operator = Table::comparisonOperator(keyword);
    if (comparison_operator.codes) {
        compiler.getNextChar();
        compiler.skipWhiteSpace();
        return relationCodes(comparison_operator);
    }
    return nullptr;
}

OperatorCodes *ComparisonOperatorCodes::relationCodes(const ComparisonOperator &compare_operator)
{
    if (compare_operator.precedence == Precedence::Relation) {
        return compare_operator.codes;
    } else {
        compiler.setEqualityCodes(compare_operator.codes);
        one_char_codes = nullptr;
        return nullptr;
    }
}

void Compiler::setEqualityCodes(OperatorCodes *codes)
{
    equality_codes = codes;
}

FunctionCodes *Compiler::getNumFunctionCodes()
{
    auto codes = Table::numFunctionCodes(word);
    if (codes) {
        clearWord();
    }
    return codes;
}

ci_string Compiler::getKeyword()
{
    if (word.empty()) {
        skipWhiteSpace();
        word_column = column;
        word = getAlphaOnlyWord();
    }
    return word;
}

void Compiler::parseKeyword(char first_char)
{
    word_column = column - 1;
    auto remaining_word = getAlphaOnlyWord();
    word += first_char;
    word += remaining_word;
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
    return word.empty() ? column : word_column;
}

void Compiler::addInstruction(Code &code)
{
    last_operand_was_constant = false;
    code_line.emplace_back(code);
}

DataType Compiler::addNumConstInstruction(bool floating_point, const std::string &number,
    unsigned column)
{
    auto const_num_info = program.constNumDictionary().add(floating_point, number);
    last_operand_was_constant = true;
    last_constant_column = column;
    last_constant_length = number.length();
    code_line.emplace_back(const_num_info.code_value);
    code_line.emplace_back(const_num_info.operand);
    return const_num_info.data_type;
}

void Compiler::convertToDouble(DataType operand_data_type)
{
    if (operand_data_type == DataType::Integer) {
        if (last_operand_was_constant) {
            changeConstantToDouble();
        } else {
            extern Code cvtdbl_code;
            addInstruction(cvtdbl_code);
        }
    }
}

void Compiler::changeConstantToDouble()
{
    extern Code const_dbl_code;

    auto last_constant_offset = code_line.size() - 2;
    code_line[last_constant_offset] = ProgramWord {const_dbl_code};
}

void Compiler::convertToInteger(DataType operand_data_type)
{
    if (operand_data_type == DataType::Double) {
        if (last_operand_was_constant) {
            changeConstantToInteger();
        } else {
            extern Code cvtint_code;
            addInstruction(cvtint_code);
        }
    }
}

void Compiler::changeConstantToInteger()
{
    extern Code const_int_code;

    auto last_constant_offset = code_line.size() - 2;
    validateConstantConvertibleToInteger(last_constant_offset);
    code_line[last_constant_offset] = ProgramWord {const_int_code};
}

void Compiler::validateConstantConvertibleToInteger(size_t last_constant_offset)
{
    auto constant_operand = code_line[last_constant_offset + 1].operand();
    if (!program.constNumDictionary().convertibleToInteger(constant_operand)) {
        throw CompileError {"floating point constant is out of range", last_constant_column,
            last_constant_length};
    }
}

ProgramCode &&Compiler::getCodeLine()
{
    return std::move(code_line);
}
