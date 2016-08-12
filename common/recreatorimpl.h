/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#ifndef IBC_RECREATORIMPL_H
#define IBC_RECREATORIMPL_H

#include "precedence.h"
#include "recreator.h"


class RecreatorImpl : public Recreator {
public:
    RecreatorImpl(const ProgramUnit &program, ProgramReader program_reader, unsigned error_offset);

    std::string &&recreate() override;
    std::string getConstNumOperand() const override;
    void addCommandKeyword(CommandCode command_code) override;
    void push(const std::string &operand) override;

    void recreateUnaryOperator() override;
    void recreateBinaryOperator() override;
    void markOperandIfError() override;

private:
    struct StackItem {
        StackItem(const std::string &string, Precedence::Level precedence);
        bool isUnaryOperator() const;

        std::string string;
        Precedence::Level precedence;
        Precedence::Level unary_operator_precedence;
    };

    void setAtErrorOffset();
    void recreateOneCode();
    std::string &&moveTopString();
    void prependKeyword(CommandCode command_code);
    Precedence::Level topPrecedence() const;
    Precedence::Level topUnaryOperatorPrecedence() const;
    void pop();
    void append(char c);
    void append(const std::string &string);
    void swapTop(std::string &string);
    void setTopPrecedence(Precedence::Level precedence);
    void setTopUnaryOperatorPrecedence(Precedence::Level precedence);
    void markErrorStart();
    void markErrorEnd();
    void appendErrorMarker(char error_marker);
    void appendUnaryOperator();
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


#endif  // IBC_RECREATORIMPL_H
