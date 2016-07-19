/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_RECREATOR_H
#define IBC_RECREATOR_H

#include <stack>
#include <string>

#include "precedence.h"
#include "programreader.h"


class CommandCode;
class ProgramUnit;

class Recreator {
public:
    Recreator(const ProgramUnit &program, ProgramReader program_reader, unsigned error_offset);
    std::string &&operator()();
    std::string getConstNumOperand() const;
    void pushKeyword(CommandCode command_code);
    void push(const std::string &operand);
    bool empty() const;
    Precedence::Level topPrecedence() const;
    bool topUnaryOperator() const;
    void pop();
    void prependKeyword(CommandCode command_code);
    void append(char c);
    void append(const std::string &string);
    void swapTop(std::string &string);
    void setTopPrecedence(Precedence::Level precedence);
    void setTopUnaryOperator(bool unary_operator);
    void markErrorStart();
    void markErrorEnd();

    void recreateUnaryOperator();
    void recreateBinaryOperator();

private:
    struct StackItem {
        StackItem(const std::string &string, Precedence::Level precedence);

        std::string string;
        Precedence::Level precedence;
        bool unary_operator;
    };

    void setAtErrorOffset();
    void recreateOneCode();
    std::string &&topString();
    void appendErrorMarker(char error_marker);
    void appendUnaryOperand(std::string &&operand, Precedence::Level operator_precedence);
    void appendSpaceForConstant(char first_char);
    void appendLeftOperand(Precedence::Level operator_precedence);
    void appendBinaryOperator();
    void appendRightOperand(const StackItem &rhs, Precedence::Level operator_precedence);
    void appendWithParens(const std::string &string);
    const char *getOperatorKeyword() const;
    Precedence::Level getOperatorPrecedence() const;

    const ProgramUnit &program;
    mutable ProgramReader program_reader;
    WordType code_value;
    unsigned error_offset;
    std::stack<StackItem> stack;
    bool at_error_offset;
};

inline Recreator::StackItem::StackItem(const std::string &string, Precedence::Level precedence) :
    string {string},
    precedence {precedence},
    unary_operator {false}
{
}


void recreateUnaryOperator(Recreator &recreator);
void recreateBinaryOperator(Recreator &recreator);
void recreateNothing(Recreator &recreator);


#endif  // IBC_RECREATOR_H
