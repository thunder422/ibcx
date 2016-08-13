/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <cctype>
#include <iostream>

#include "compiler.h"
#include "constnumcompiler.h"
#include "compileerror.h"
#include "programcode.h"
#include "programunit.h"


class ConstNumCompilerImpl : public ConstNumCompiler {
public:
    ConstNumCompilerImpl(Compiler &compiler);

    DataType compile() override;
    bool negateOperator() const noexcept override;
    char unparsedChar() const noexcept override;

private:
    void parseInput();

    void parseStartState(int next_char);
    void parseNegativeState(int next_char);
    void parseZeroState(int next_char);
    void parsePeriodState(int next_char);
    void parseMantissaState(int next_char);
    void parseExponentState(int next_char);
    void parseExponentSignState(int next_char);
    void parseExponentDigitsState(int next_char);

    bool validMantissaChar(int next_char) noexcept;
    void addNextChar();
    void setNegateOperator() noexcept;
    void removeExponentChar() noexcept;

    Compiler &compiler;

    void (ConstNumCompilerImpl::*parse_state)(int next_char);
    std::string number;
    unsigned first_column;
    bool floating_point {false};
    bool done {false};
    bool negate_operator {false};
    char unparsed_char {0};
};

// ----------------------------------------

std::unique_ptr<ConstNumCompiler> ConstNumCompiler::create(Compiler &compiler)
{
    return std::unique_ptr<ConstNumCompiler> {new ConstNumCompilerImpl {compiler}};
}

// ----------------------------------------

ConstNumCompilerImpl::ConstNumCompilerImpl(Compiler &compiler) :
    compiler {compiler},
    parse_state {&ConstNumCompilerImpl::parseStartState},
    first_column {compiler.getColumn()}
{
}

DataType ConstNumCompilerImpl::compile()
{
    parseInput();
    if (number.empty()) {
        return DataType::Null;
    } else {
        try {
            return compiler.addNumConstInstruction(floating_point, number, first_column);
        }
        catch (const std::out_of_range &) {
            unsigned length = number.length();
            throw CompileError {"floating point constant is out of range", first_column, length};
        }
    }
}

bool ConstNumCompilerImpl::negateOperator() const noexcept
{
    return negate_operator;
}

char ConstNumCompilerImpl::unparsedChar() const noexcept
{
    return unparsed_char;
}

// ----------------------------------------

void ConstNumCompilerImpl::parseInput()
{
    do {
        auto next_char = compiler.peekNextChar();
        (this->*parse_state)(next_char);
    } while (!done);
}

void ConstNumCompilerImpl::parseStartState(int next_char)
{
    if (next_char == '0') {
        parse_state = &ConstNumCompilerImpl::parseZeroState;
    } else if (next_char == '-') {
        parse_state = &ConstNumCompilerImpl::parseNegativeState;
    } else if (!validMantissaChar(next_char)) {
        done = true;
        return;
    }
    addNextChar();
}

void ConstNumCompilerImpl::parseNegativeState(int next_char)
{
    if (validMantissaChar(next_char)) {
        addNextChar();
    } else {
        setNegateOperator();
    }
}

void ConstNumCompilerImpl::parseZeroState(int next_char)
{
    if (next_char == '.') {
        parse_state = &ConstNumCompilerImpl::parseMantissaState;
    } else if (isdigit(next_char)) {
        throw CompileError {"expected decimal point after leading zero", compiler.getColumn()};
    } else {
        done = true;
    }
}

void ConstNumCompilerImpl::parsePeriodState(int next_char)
{
    if (isdigit(next_char)) {
        parse_state = &ConstNumCompilerImpl::parseMantissaState;
    } else {
        throw CompileError {"expected digit after decimal point", compiler.getColumn()};
    }
}

void ConstNumCompilerImpl::parseMantissaState(int next_char)
{
    if (next_char == '.' && !floating_point) {
        floating_point = true;
    } else if (toupper(next_char) == 'E') {
        parse_state = &ConstNumCompilerImpl::parseExponentState;
    } else if (!isdigit(next_char)) {
        done = true;
        return;
    }
    addNextChar();
}

void ConstNumCompilerImpl::parseExponentState(int next_char)
{
    if (next_char == '-' || next_char == '+') {
        parse_state = &ConstNumCompilerImpl::parseExponentSignState;
    } else if (isdigit(next_char)) {
        parse_state = &ConstNumCompilerImpl::parseExponentDigitsState;
    } else if (isalpha(next_char)) {
        removeExponentChar();
        return;
    } else {
        throw CompileError {"expected sign or digit for exponent", compiler.getColumn()};
    }
    floating_point = true;
    addNextChar();
}

void ConstNumCompilerImpl::parseExponentSignState(int next_char)
{
    if (isdigit(next_char)) {
        parse_state = &ConstNumCompilerImpl::parseExponentDigitsState;
    } else {
        throw CompileError {"expected digit after exponent sign", compiler.getColumn()};
    }
}

void ConstNumCompilerImpl::parseExponentDigitsState(int next_char)
{
    if (isdigit(next_char)) {
        addNextChar();
    } else {
        done = true;
    }
}

// ----------------------------------------

void ConstNumCompilerImpl::addNextChar()
{
    number += compiler.getNextChar();
}

bool ConstNumCompilerImpl::validMantissaChar(int next_char) noexcept
{
    if (next_char == '.') {
        floating_point = true;
        parse_state = &ConstNumCompilerImpl::parsePeriodState;
    } else if (isdigit(next_char)) {
        parse_state = &ConstNumCompilerImpl::parseMantissaState;
    } else {
        return false;
    }
    return true;
}

void ConstNumCompilerImpl::setNegateOperator() noexcept
{
    negate_operator = true;
    number.clear();
    done = true;
}

void ConstNumCompilerImpl::removeExponentChar() noexcept
{
    unparsed_char = number.back();
    number.pop_back();
    done = true;
}
