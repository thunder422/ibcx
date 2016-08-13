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


class ConstNumCompiler::Impl {
public:
    Impl(Compiler &compiler);

    DataType compile();
    bool negateOperator() const noexcept;
    char nextChar() const noexcept;

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

    void (Impl::*parse_state)(int next_char);
    std::string number;
    unsigned first_column;
    bool floating_point {false};
    bool done {false};
    bool negate_operator {false};
    char next_char {0};
};

// ----------------------------------------

ConstNumCompiler::ConstNumCompiler(Compiler &compiler) :
    pimpl {new Impl(compiler)}
{
}

DataType ConstNumCompiler::operator()()
{
    return pimpl->compile();
}

bool ConstNumCompiler::negateOperator() const noexcept
{
    return pimpl->negateOperator();
}

char ConstNumCompiler::nextChar() const noexcept
{
    return pimpl->nextChar();
}

ConstNumCompiler::~ConstNumCompiler()
{
}

// ----------------------------------------

ConstNumCompiler::Impl::Impl(Compiler &compiler) :
    compiler {compiler},
    parse_state {&Impl::parseStartState},
    first_column {compiler.getColumn()}
{
}

DataType ConstNumCompiler::Impl::compile()
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

bool ConstNumCompiler::Impl::negateOperator() const noexcept
{
    return negate_operator;
}

char ConstNumCompiler::Impl::nextChar() const noexcept
{
    return next_char;
}

// ----------------------------------------

void ConstNumCompiler::Impl::parseInput()
{
    do {
        auto next_char = compiler.peekNextChar();
        (this->*parse_state)(next_char);
    } while (!done);
}

void ConstNumCompiler::Impl::parseStartState(int next_char)
{
    if (next_char == '0') {
        parse_state = &Impl::parseZeroState;
    } else if (next_char == '-') {
        parse_state = &Impl::parseNegativeState;
    } else if (!validMantissaChar(next_char)) {
        done = true;
        return;
    }
    addNextChar();
}

void ConstNumCompiler::Impl::parseNegativeState(int next_char)
{
    if (validMantissaChar(next_char)) {
        addNextChar();
    } else {
        setNegateOperator();
    }
}

void ConstNumCompiler::Impl::parseZeroState(int next_char)
{
    if (next_char == '.') {
        parse_state = &Impl::parseMantissaState;
    } else if (isdigit(next_char)) {
        throw CompileError {"expected decimal point after leading zero", compiler.getColumn()};
    } else {
        done = true;
    }
}

void ConstNumCompiler::Impl::parsePeriodState(int next_char)
{
    if (isdigit(next_char)) {
        parse_state = &Impl::parseMantissaState;
    } else {
        throw CompileError {"expected digit after decimal point", compiler.getColumn()};
    }
}

void ConstNumCompiler::Impl::parseMantissaState(int next_char)
{
    if (next_char == '.' && !floating_point) {
        floating_point = true;
    } else if (toupper(next_char) == 'E') {
        parse_state = &Impl::parseExponentState;
    } else if (!isdigit(next_char)) {
        done = true;
        return;
    }
    addNextChar();
}

void ConstNumCompiler::Impl::parseExponentState(int next_char)
{
    if (next_char == '-' || next_char == '+') {
        parse_state = &Impl::parseExponentSignState;
    } else if (isdigit(next_char)) {
        parse_state = &Impl::parseExponentDigitsState;
    } else if (isalpha(next_char)) {
        removeExponentChar();
        return;
    } else {
        throw CompileError {"expected sign or digit for exponent", compiler.getColumn()};
    }
    floating_point = true;
    addNextChar();
}

void ConstNumCompiler::Impl::parseExponentSignState(int next_char)
{
    if (isdigit(next_char)) {
        parse_state = &Impl::parseExponentDigitsState;
    } else {
        throw CompileError {"expected digit after exponent sign", compiler.getColumn()};
    }
}

void ConstNumCompiler::Impl::parseExponentDigitsState(int next_char)
{
    if (isdigit(next_char)) {
        addNextChar();
    } else {
        done = true;
    }
}

// ----------------------------------------

void ConstNumCompiler::Impl::addNextChar()
{
    number += compiler.getNextChar();
}

bool ConstNumCompiler::Impl::validMantissaChar(int next_char) noexcept
{
    if (next_char == '.') {
        floating_point = true;
        parse_state = &Impl::parsePeriodState;
    } else if (isdigit(next_char)) {
        parse_state = &Impl::parseMantissaState;
    } else {
        return false;
    }
    return true;
}

void ConstNumCompiler::Impl::setNegateOperator() noexcept
{
    negate_operator = true;
    number.clear();
    done = true;
}

void ConstNumCompiler::Impl::removeExponentChar() noexcept
{
    next_char = number.back();
    number.pop_back();
    done = true;
}
