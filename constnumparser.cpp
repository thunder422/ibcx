/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <cctype>
#include <iostream>

#include "constnumparser.h"
#include "programcode.h"


class State {
public:
    virtual void process(ConstNumParser &parser, int c) const = 0;
};

class StartState : public State {
public:
    static StartState *instance();
    void process(ConstNumParser &parser, int c) const override;
    bool done() const;
};

// ----------------------------------------

ConstNumParser::ConstNumParser(std::istream &is_) :
    is {is_},
    state {StartState::instance()},
    done {false}
{
}

UniqueToken ConstNumParser::getToken()
{
    processInput();
    return number.empty() ? UniqueToken() : UniqueToken(new Token);
}

DataType ConstNumParser::getCode(ProgramCode &code)
{
    (void)code;
    return DataType::Null;
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

StartState *StartState::instance()
{
    static StartState start_state;
    return &start_state;
}

void StartState::process(ConstNumParser &parser, int c) const
{
    if (isdigit(c)) {
        parser.add(c);
    }
    parser.setDone();
}
