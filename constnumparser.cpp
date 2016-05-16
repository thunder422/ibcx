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
#include "programcode.h"
#include "programmodel.h"


class State {
public:
    virtual void process(ConstNumParser &parser, int next_char) const = 0;
};

class StartState : public State {
public:
    static State *instance();
    void process(ConstNumParser &parser, int next_char) const override;
    bool done() const;
};

class MantissaState : public State {
public:
    static State *instance();
    void process(ConstNumParser &parser, int next_char) const override;
};

class ExponentState : public State {
public:
    static State *instance();
    void process(ConstNumParser &parser, int next_char) const override;
};

// ----------------------------------------

ConstNumParser::ConstNumParser(std::istream &is_) :
    is {is_},
    state {StartState::instance()},
    floating_point {false},
    done {false}
{
}

Code constDblCode;
Code constIntCode;

DataType ConstNumParser::getCode(ProgramCode &code, ProgramModel &program)
{
    processInput();
    if (number.empty()) {
        return DataType::Null;
    } else if (floating_point) {
        code.emplace_back(constDblCode);
        code.emplace_back(program.constDblDictionary().add(number));
        return DataType::Double;
    } else {
        code.emplace_back(constIntCode);
        code.emplace_back(program.constIntDictionary().add(number));
        return DataType::Integer;
    }
}

void ConstNumParser::processInput()
{
    do {
        auto next_char = is.peek();
        state->process(*this, next_char);
    } while (!done);
}

void ConstNumParser::changeState(State *state_)
{
    state = state_;
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

// ----------------------------------------

State *StartState::instance()
{
    static StartState start_state;
    return &start_state;
}

void StartState::process(ConstNumParser &parser, int next_char) const
{
    if (next_char == '.') {
        parser.setDouble();
    } else if (!isdigit(next_char) && next_char != '-') {
        parser.setDone();
        return;
    }
    parser.changeState(MantissaState::instance());
    parser.addNextChar();
}

// ----------------------------------------

State *MantissaState::instance()
{
    static MantissaState mantissa_state;
    return &mantissa_state;
}

void MantissaState::process(ConstNumParser &parser, int next_char) const
{
    if (next_char == '.' && !parser.isDouble()) {
        parser.setDouble();
    } else if (toupper(next_char) == 'E') {
        parser.changeState(ExponentState::instance());
        parser.setDouble();
    } else if (!isdigit(next_char)) {
        parser.setDone();
        return;
    }
    parser.addNextChar();
}

// ----------------------------------------

State *ExponentState::instance()
{
    static ExponentState expononent_state;
    return &expononent_state;

}

void ExponentState::process(ConstNumParser &parser, int next_char) const
{
    if (isdigit(next_char)) {
        parser.addNextChar();
    } else if (next_char == '-') {
        parser.addNextChar();
    } else {
        parser.setDone();
    }
}
