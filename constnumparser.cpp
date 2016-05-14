/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <cctype>
#include <iostream>

#include "constnumparser.h"


class State {
public:
    virtual void process(ConstNumParser &parser, int c) const = 0;
    virtual bool done() const = 0;
};

class StartState : public State {
public:
    static StartState *instance();
    void process(ConstNumParser &parser, int c) const override;
    bool done() const;
};


class EndState : public State {
public:
    static EndState *instance();
    void process(ConstNumParser &parser, int c) const override;
    bool done() const;
};

// ----------------------------------------

ConstNumParser::ConstNumParser(std::istream &is_) :
    is(is_),
    state(StartState::instance())
{
}

UniqueToken ConstNumParser::getToken()
{
    processInput();
    return number.empty() ? UniqueToken() : UniqueToken(new Token);
}

void ConstNumParser::processInput()
{
    do {
        auto c = is.get();
        state->process(*this, c);
    } while (!state->done());
}

void ConstNumParser::changeState(State *state_)
{
    state = state_;
}

void ConstNumParser::add(char c)
{
    number += c;
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
        parser.changeState(EndState::instance());
        parser.add(c);
    }
}

bool StartState::done() const
{
    return true;
}

// ----------------------------------------

EndState *EndState::instance()
{
    static EndState end_state;
    return &end_state;
}

void EndState::process(ConstNumParser &parser, int c) const
{
    (void)parser;
    (void)c;
}

bool EndState::done() const
{
    return true;
}
