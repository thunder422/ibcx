/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <cctype>
#include <iostream>

#include "code.h"
#include "constnumparser.h"
#include "parseerror.h"
#include "programcode.h"
#include "programunit.h"


class State;

class ConstNumParser::Impl {
public:
    Impl(std::istream &is, ProgramCode &code_line, ProgramUnit &program);

    DataType parse();
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
    void processInput();

    std::istream &is;
    ProgramCode &code_line;
    ProgramUnit &program;

    State *state;
    std::string number;
    bool floating_point;
    bool done;
    bool negate_operator;
    bool possible_operator;
    unsigned column;
};

// ----------------------------------------

ConstNumParser::ConstNumParser(std::istream &is, ProgramCode &code_line, ProgramUnit &program) :
    pimpl {new Impl(is, code_line, program)}
{
}

DataType ConstNumParser::operator()()
{
    return pimpl->parse();
}

bool ConstNumParser::negateOperator() const noexcept
{
    return pimpl->negateOperator();
}

bool ConstNumParser::possibleOperator() const noexcept
{
    return pimpl->possibleOperator();
}

ConstNumParser::~ConstNumParser()
{
}

// ----------------------------------------

class State {
public:
    virtual void process(ConstNumParser::Impl &parser, int next_char) const = 0;

protected:
    bool validMantissaChar(ConstNumParser::Impl &parser, int next_char) const;
};

class StartState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
};

class NegativeState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
};

class ZeroState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
};

class PeriodState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
};

class MantissaState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
};

class ExponentState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
};

class ExponentSignState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
};

class ExponentDigitsState : public State {
public:
    void process(ConstNumParser::Impl &parser, int next_char) const override;
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

ConstNumParser::Impl::Impl(std::istream &is, ProgramCode &code_line, ProgramUnit &program) :
    is {is},
    code_line {code_line},
    program {program},
    state {&start},
    floating_point {false},
    done {false},
    negate_operator {false},
    possible_operator {false}
{
}

Code const_dbl_code;
Code const_int_code;

DataType ConstNumParser::Impl::parse()
{
    processInput();
    if (number.empty()) {
        return DataType::Null;
    } else if (floating_point) {
        code_line.emplace_back(const_dbl_code);
        code_line.emplace_back(program.constDblDictionary().add(number));
        return DataType::Double;
    } else {
        code_line.emplace_back(const_int_code);
        code_line.emplace_back(program.constIntDictionary().add(number));
        return DataType::Integer;
    }
}

void ConstNumParser::Impl::processInput()
{
    do {
        column = is.tellg();
        auto next_char = is.peek();
        state->process(*this, next_char);
    } while (!done);
}

bool ConstNumParser::Impl::negateOperator() const noexcept
{
    return negate_operator;
}

bool ConstNumParser::Impl::possibleOperator() const noexcept
{
    return possible_operator;
}

void ConstNumParser::Impl::changeState(State &new_state) noexcept
{
    state = &new_state;
}

unsigned ConstNumParser::Impl::getColumn() const noexcept
{
    return column;
}

void ConstNumParser::Impl::addNextChar()
{
    number += is.get();
}

void ConstNumParser::Impl::setDouble() noexcept
{
    floating_point = true;
}

bool ConstNumParser::Impl::isDouble() const noexcept
{
    return floating_point;
}

void ConstNumParser::Impl::setDone() noexcept
{
    done = true;
}

void ConstNumParser::Impl::setNegateOperator() noexcept
{
    negate_operator = true;
    number.clear();
    setDone();
}

void ConstNumParser::Impl::setPossibleOperator() noexcept
{
    possible_operator = true;
    number.pop_back();
    setDone();
}

// ----------------------------------------

void StartState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (next_char == '0') {
        parser.changeState(zero);
    } else if (next_char == '-') {
        parser.changeState(negative);
    } else if (!validMantissaChar(parser, next_char)) {
        parser.setDone();
        return;
    }
    parser.addNextChar();
}

void NegativeState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (validMantissaChar(parser, next_char)) {
        parser.addNextChar();
    } else {
        parser.setNegateOperator();
    }
}

bool State::validMantissaChar(ConstNumParser::Impl &parser, int next_char) const
{
    if (next_char == '.') {
        parser.setDouble();
        parser.changeState(period);
    } else if (isdigit(next_char)) {
        parser.changeState(mantissa);
    } else {
        return false;
    }
    return true;
}

void ZeroState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (next_char == '.') {
        parser.changeState(mantissa);
    } else if (isdigit(next_char)) {
        throw ParseError {"expected decimal point after leading zero", parser.getColumn()};
    } else {
        parser.setDone();
    }
}

void PeriodState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (isdigit(next_char)) {
        parser.changeState(mantissa);
    } else {
        throw ParseError {"expected digit after decimal point", parser.getColumn()};
    }
}

void MantissaState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (next_char == '.' && !parser.isDouble()) {
        parser.setDouble();
    } else if (toupper(next_char) == 'E') {
        parser.changeState(exponent);
    } else if (!isdigit(next_char)) {
        parser.setDone();
        return;
    }
    parser.addNextChar();
}

void ExponentState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (next_char == '-' || next_char == '+') {
        parser.changeState(exponent_sign);
    } else if (isdigit(next_char)) {
        parser.changeState(exponent_digits);
    } else if (isalpha(next_char)) {
        parser.setPossibleOperator();
        return;
    } else {
        throw ParseError {"expected sign or digit for exponent", parser.getColumn()};
    }
    parser.setDouble();
    parser.addNextChar();
}

void ExponentSignState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (isdigit(next_char)) {
        parser.changeState(exponent_digits);
    } else {
        throw ParseError {"expected digit after exponent sign", parser.getColumn()};
    }
}

void ExponentDigitsState::process(ConstNumParser::Impl &parser, int next_char) const
{
    if (isdigit(next_char)) {
        parser.addNextChar();
    } else {
        parser.setDone();
    }
}
