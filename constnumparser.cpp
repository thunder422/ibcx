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
    virtual void process(ConstNumParser &parser, int c) const = 0;
};

class StartState : public State {
public:
    static State *instance();
    void process(ConstNumParser &parser, int c) const override;
    bool done() const;
};

class MantissaState : public State {
public:
    static State *instance();
    void process(ConstNumParser &parser, int c) const override;
};

// ----------------------------------------

ConstNumParser::ConstNumParser(std::istream &is_) :
    is {is_},
    state {StartState::instance()},
    done {false}
{
}

Code constIntCode;

DataType ConstNumParser::getCode(ProgramCode &code, ProgramModel &program)
{
    processInput();
    if (number.empty()) {
        return DataType::Null;
    }
    code.emplace_back(constIntCode);
    code.emplace_back(program.constIntDictionary().add(number));
    return DataType::Integer;
}

void ConstNumParser::processInput()
{
    do {
        auto c = is.get();
        state->process(*this, c);
    } while (!done);
}

void ConstNumParser::changeState(State *state_)
{
    state = state_;
}

void ConstNumParser::add(char c)
{
    number += c;
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

void StartState::process(ConstNumParser &parser, int c) const
{
    if (isdigit(c)) {
        parser.changeState(MantissaState::instance());
        parser.add(c);
    } else {
        parser.setDone();
    }
}

// ----------------------------------------

State *MantissaState::instance()
{
    static MantissaState mantissa_state;
    return &mantissa_state;
}

void MantissaState::process(ConstNumParser &parser, int c) const
{
    if (isdigit(c)) {
        parser.add(c);
    } else {
        parser.setDone();
    }
}
