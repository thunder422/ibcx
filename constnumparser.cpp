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


class State {
public:
    virtual void process(ConstNumParser &parser, int next_char) const = 0;

protected:
    bool validMantissaChar(ConstNumParser &parser, int next_char) const;
};

class StartState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
};

class NegativeState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
};

class ZeroState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
};

class PeriodState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
};

class MantissaState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
};

class ExponentState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
};

class ExponentSignState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
};

class ExponentDigitsState : public State {
public:
    void process(ConstNumParser &parser, int next_char) const override;
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

ConstNumParser::ConstNumParser(std::istream &is) :
    is {is},
    state {&start},
    floating_point {false},
    done {false},
    negate_operator {false}
{
}

Code constDblCode;
Code constIntCode;

DataType ConstNumParser::parse(ProgramCode &code_line, ProgramUnit &program)
{
    processInput();
    if (number.empty()) {
        return DataType::Null;
    } else if (floating_point) {
        code_line.emplace_back(constDblCode);
        code_line.emplace_back(program.constDblDictionary().add(number));
        return DataType::Double;
    } else {
        code_line.emplace_back(constIntCode);
        code_line.emplace_back(program.constIntDictionary().add(number));
        return DataType::Integer;
    }
}

bool ConstNumParser::negateOperator() const
{
    return negate_operator;
}

bool ConstNumParser::possibleOperator() const
{
    return false;
}

void ConstNumParser::processInput()
{
    do {
        column = is.tellg();
        auto next_char = is.peek();
        state->process(*this, next_char);
    } while (!done);
}

void ConstNumParser::changeState(State &new_state)
{
    state = &new_state;
}

unsigned ConstNumParser::getColumn() const
{
    return column;
}

void ConstNumParser::addNextChar()
{
    number += is.get();
}

void ConstNumParser::setDouble()
{
    floating_point = true;
}

bool ConstNumParser::isDouble()
{
    return floating_point;
}

void ConstNumParser::setDone()
{
    done = true;
}

void ConstNumParser::setNegateOperator()
{
    negate_operator = true;
    number.clear();
    setDone();
}

void ConstNumParser::setPossibleOperator()
{
    number.pop_back();
    setDone();
}

// ----------------------------------------

void StartState::process(ConstNumParser &parser, int next_char) const
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

void NegativeState::process(ConstNumParser &parser, int next_char) const
{
    if (validMantissaChar(parser, next_char)) {
        parser.addNextChar();
    } else {
        parser.setNegateOperator();
    }
}

bool State::validMantissaChar(ConstNumParser &parser, int next_char) const
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

void ZeroState::process(ConstNumParser &parser, int next_char) const
{
    if (next_char == '.') {
        parser.changeState(mantissa);
    } else if (isdigit(next_char)) {
        throw ParseError {"expected decimal point after leading zero", parser.getColumn()};
    } else {
        parser.setDone();
    }
}

void PeriodState::process(ConstNumParser &parser, int next_char) const
{
    if (isdigit(next_char)) {
        parser.changeState(mantissa);
    } else {
        throw ParseError {"expected digit after decimal point", parser.getColumn()};
    }
}

void MantissaState::process(ConstNumParser &parser, int next_char) const
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

void ExponentState::process(ConstNumParser &parser, int next_char) const
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

void ExponentSignState::process(ConstNumParser &parser, int next_char) const
{
    if (isdigit(next_char)) {
        parser.changeState(exponent_digits);
    } else {
        throw ParseError {"expected digit after exponent sign", parser.getColumn()};
    }
}

void ExponentDigitsState::process(ConstNumParser &parser, int next_char) const
{
    if (isdigit(next_char)) {
        parser.addNextChar();
    } else {
        parser.setDone();
    }
}
