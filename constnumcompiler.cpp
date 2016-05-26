/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <cctype>
#include <iostream>

#include "code.h"
#include "compiler.h"
#include "constnumcompiler.h"
#include "compileerror.h"
#include "programcode.h"
#include "programunit.h"


class State;

class ConstNumCompiler::Impl {
public:
    Impl(Compiler &compiler);

    DataType compile();
    bool negateOperator() const noexcept;
    bool possibleOperator() const noexcept;

    void changeState(State &new_state) noexcept;
    unsigned getColumn() const noexcept;
    void addNextChar();
    void setDouble() noexcept;
    bool isDouble() const noexcept;
    void setDone() noexcept;
    void setNegateOperator() noexcept;
    void setPossibleOperator() noexcept;

private:
    void parseInput();

    Compiler &compiler;

    State *state;
    std::string number;
    bool floating_point;
    bool done;
    bool negate_operator;
    bool possible_operator;
    unsigned column;
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

bool ConstNumCompiler::possibleOperator() const noexcept
{
    return pimpl->possibleOperator();
}

ConstNumCompiler::~ConstNumCompiler()
{
}

// ----------------------------------------

class State {
public:
    virtual void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const = 0;

protected:
    bool validMantissaChar(ConstNumCompiler::Impl &constant_compiler, int next_char) const;
};

class StartState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};

class NegativeState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};

class ZeroState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};

class PeriodState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};

class MantissaState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};

class ExponentState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};

class ExponentSignState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};

class ExponentDigitsState : public State {
public:
    void parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const override;
};


static StartState start;
static ZeroState zero;
static NegativeState negative;
static PeriodState period;
static MantissaState mantissa;
static ExponentState exponent;
static ExponentSignState exponent_sign;
static ExponentDigitsState exponent_digits;

// ----------------------------------------

ConstNumCompiler::Impl::Impl(Compiler &compiler) :
    compiler {compiler},
    state {&start},
    floating_point {false},
    done {false},
    negate_operator {false},
    possible_operator {false}
{
}

Code const_dbl_code;
Code const_int_code;

DataType ConstNumCompiler::Impl::compile()
{
    parseInput();
    if (number.empty()) {
        return DataType::Null;
    } else if (floating_point) {
        compiler.addConstNumInstruction(const_dbl_code, number);
        return DataType::Double;
    } else {
        compiler.addConstNumInstruction(const_int_code, number);
        return DataType::Integer;
    }
}

void ConstNumCompiler::Impl::parseInput()
{
    do {
        column = compiler.getColumn();
        auto next_char = compiler.peekNextChar();
        state->parse(*this, next_char);
    } while (!done);
}

bool ConstNumCompiler::Impl::negateOperator() const noexcept
{
    return negate_operator;
}

bool ConstNumCompiler::Impl::possibleOperator() const noexcept
{
    return possible_operator;
}

void ConstNumCompiler::Impl::changeState(State &new_state) noexcept
{
    state = &new_state;
}

unsigned ConstNumCompiler::Impl::getColumn() const noexcept
{
    return column;
}

void ConstNumCompiler::Impl::addNextChar()
{
    number += compiler.getNextChar();
}

void ConstNumCompiler::Impl::setDouble() noexcept
{
    floating_point = true;
}

bool ConstNumCompiler::Impl::isDouble() const noexcept
{
    return floating_point;
}

void ConstNumCompiler::Impl::setDone() noexcept
{
    done = true;
}

void ConstNumCompiler::Impl::setNegateOperator() noexcept
{
    negate_operator = true;
    number.clear();
    setDone();
}

void ConstNumCompiler::Impl::setPossibleOperator() noexcept
{
    possible_operator = true;
    number.pop_back();
    setDone();
}

// ----------------------------------------

void StartState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (next_char == '0') {
        constant_compiler.changeState(zero);
    } else if (next_char == '-') {
        constant_compiler.changeState(negative);
    } else if (!validMantissaChar(constant_compiler, next_char)) {
        constant_compiler.setDone();
        return;
    }
    constant_compiler.addNextChar();
}

void NegativeState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (validMantissaChar(constant_compiler, next_char)) {
        constant_compiler.addNextChar();
    } else {
        constant_compiler.setNegateOperator();
    }
}

bool State::validMantissaChar(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (next_char == '.') {
        constant_compiler.setDouble();
        constant_compiler.changeState(period);
    } else if (isdigit(next_char)) {
        constant_compiler.changeState(mantissa);
    } else {
        return false;
    }
    return true;
}

void ZeroState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (next_char == '.') {
        constant_compiler.changeState(mantissa);
    } else if (isdigit(next_char)) {
        throw CompileError {"expected decimal point after leading zero",
            constant_compiler.getColumn()};
    } else {
        constant_compiler.setDone();
    }
}

void PeriodState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (isdigit(next_char)) {
        constant_compiler.changeState(mantissa);
    } else {
        throw CompileError {"expected digit after decimal point", constant_compiler.getColumn()};
    }
}

void MantissaState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (next_char == '.' && !constant_compiler.isDouble()) {
        constant_compiler.setDouble();
    } else if (toupper(next_char) == 'E') {
        constant_compiler.changeState(exponent);
    } else if (!isdigit(next_char)) {
        constant_compiler.setDone();
        return;
    }
    constant_compiler.addNextChar();
}

void ExponentState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (next_char == '-' || next_char == '+') {
        constant_compiler.changeState(exponent_sign);
    } else if (isdigit(next_char)) {
        constant_compiler.changeState(exponent_digits);
    } else if (isalpha(next_char)) {
        constant_compiler.setPossibleOperator();
        return;
    } else {
        throw CompileError {"expected sign or digit for exponent", constant_compiler.getColumn()};
    }
    constant_compiler.setDouble();
    constant_compiler.addNextChar();
}

void ExponentSignState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (isdigit(next_char)) {
        constant_compiler.changeState(exponent_digits);
    } else {
        throw CompileError {"expected digit after exponent sign", constant_compiler.getColumn()};
    }
}

void ExponentDigitsState::parse(ConstNumCompiler::Impl &constant_compiler, int next_char) const
{
    if (isdigit(next_char)) {
        constant_compiler.addNextChar();
    } else {
        constant_compiler.setDone();
    }
}
