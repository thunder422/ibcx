/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include <stack>

#include "commandcode.h"
#include "programcode.h"
#include "programerror.h"
#include "programreader.h"
#include "programunit.h"
#include "recreatorimpl.h"


RecreatorImpl::StackItem::StackItem(const std::string &string, Precedence::Level precedence) :
    string {string},
    precedence {precedence},
    unary_operator_precedence {Precedence::Level::Operand}
{
}

bool RecreatorImpl::StackItem::isUnaryOperator() const
{
    return unary_operator_precedence != Precedence::Level::Operand;
}

// ------------------------------------------------------------

RecreatorImpl::RecreatorImpl(const ProgramUnit &program, ProgramReader program_reader,
        unsigned error_offset) :
    program {program},
    program_reader {program_reader},
    error_offset {error_offset}
{
}

std::string &&RecreatorImpl::recreate()
{
    while (program_reader.hasMoreCode()) {
        setAtErrorOffset();
        recreateOneCode();
    }
    return moveTopString();
}

void RecreatorImpl::setAtErrorOffset()
{
    at_error_offset = program_reader.currentOffset() == error_offset;
}

void RecreatorImpl::recreateOneCode()
{
    auto code = program_reader.getInstruction();
    code_value = code->getValue();
    code->recreate(*this);
}

std::string RecreatorImpl::getConstNumOperand() const
{
    auto operand = program_reader.getOperand();
    return program.constNumDictionary().get(operand);
}

void RecreatorImpl::addCommandKeyword(CommandCode command_code)
{
    if (stack.empty()) {
        push(command_code.getKeyword());
    } else {
        prependKeyword(command_code);
    }
}

void RecreatorImpl::push(const std::string &operand)
{
    stack.emplace(operand, Precedence::Level::Operand);
}

std::string &&RecreatorImpl::moveTopString()
{
    return std::move(stack.top().string);
}

Precedence::Level RecreatorImpl::topPrecedence() const
{
    return stack.top().precedence;
}

Precedence::Level RecreatorImpl::topUnaryOperatorPrecedence() const
{
    return stack.top().unary_operator_precedence;
}

void RecreatorImpl::pop()
{
    stack.pop();
}

void RecreatorImpl::prependKeyword(CommandCode command_code)
{
    std::string string = command_code.getKeyword();
    swapTop(string);
    append(' ');
    append(string);
}

void RecreatorImpl::append(char c)
{
    stack.top().string += c;
}

void RecreatorImpl::append(const std::string &string)
{
    stack.top().string += string;
}

void RecreatorImpl::swapTop(std::string &string)
{
    std::swap(stack.top().string, string);
}

void RecreatorImpl::setTopPrecedence(Precedence::Level precedence)
{
    stack.top().precedence = precedence;
}

void RecreatorImpl::setTopUnaryOperatorPrecedence(Precedence::Level precedence)
{
    stack.top().unary_operator_precedence = precedence;
}

void RecreatorImpl::markErrorStart()
{
    appendErrorMarker(StartErrorMarker);
}

void RecreatorImpl::markErrorEnd()
{
    appendErrorMarker(EndErrorMarker);
}

void RecreatorImpl::appendErrorMarker(char error_marker)
{
    if (at_error_offset) {
        append(error_marker);
    }
}

void RecreatorImpl::recreateUnaryOperator()
{
    auto operand = moveTopString();

    appendUnaryOperator();
    auto operator_precedence = getOperatorPrecedence();

    appendUnaryOperand(std::move(operand), operator_precedence);

    setTopPrecedence(operator_precedence);
    setTopUnaryOperatorPrecedence(operator_precedence);
}

void RecreatorImpl::appendUnaryOperator()
{
    markErrorStart();
    append(getOperatorKeyword());
    markErrorEnd();
    if (isalpha(moveTopString().back())) {
        append(' ');
    }
}

void RecreatorImpl::appendUnaryOperand(std::string &&operand, Precedence::Level operator_precedence)
{
    auto operand_precedence = topPrecedence();
    auto lower_precedence = operand_precedence > operator_precedence;
    if (lower_precedence) {
        appendWithParens(operand);
    } else {
        appendSpaceForConstant(operand.front());
        append(operand);
    }
}

void RecreatorImpl::appendSpaceForConstant(char first_char)
{
    if (isdigit(first_char) || first_char == '.') {
        if (moveTopString().back() != ' ') {
            append(' ');
        }
    }
}

void RecreatorImpl::recreateBinaryOperator()
{
    auto rhs = stack.top();
    pop();

    auto operator_precedence = getOperatorPrecedence();

    appendLeftOperand(operator_precedence);
    appendBinaryOperator();
    appendRightOperand(rhs, operator_precedence);

    setTopPrecedence(operator_precedence);
    setTopUnaryOperatorPrecedence(rhs.unary_operator_precedence);
}

void RecreatorImpl::appendLeftOperand(Precedence::Level operator_precedence)
{
    auto lhs_precedence = topPrecedence();
    auto lhs_unary_operator_precedence = topUnaryOperatorPrecedence();
    if (lhs_precedence > operator_precedence
            || lhs_unary_operator_precedence > operator_precedence) {
        auto left_operand = moveTopString();
        appendWithParens(left_operand);
    }
}

void RecreatorImpl::appendBinaryOperator()
{
    append(' ');
    markErrorStart();
    append(getOperatorKeyword());
    markErrorEnd();
    append(' ');
}

void RecreatorImpl::appendRightOperand(const StackItem &rhs, Precedence::Level operator_precedence)
{
    auto lower_precedence = rhs.precedence >= operator_precedence && !rhs.isUnaryOperator();
    if (lower_precedence) {
        appendWithParens(rhs.string);
    } else {
        append(rhs.string);
    }
}

void RecreatorImpl::appendWithParens(const std::string &string)
{
    append('(');
    append(string);
    append(')');
}

const char *RecreatorImpl::getOperatorKeyword() const
{
    return Precedence::getKeyword(code_value);
}

Precedence::Level RecreatorImpl::getOperatorPrecedence() const
{
    return Precedence::getPrecedence(code_value);
}

void RecreatorImpl::markOperandIfError()
{
    auto operand = moveTopString();
    markErrorStart();
    append(operand);
    markErrorEnd();
}

// ------------------------------------------------------------

std::unique_ptr<Recreator> Recreator::create(const ProgramUnit &program,
    ProgramReader program_reader, unsigned error_offset)
{
    return std::unique_ptr<Recreator> {new RecreatorImpl {program, program_reader, error_offset}};
}

// ------------------------------------------------------------

void recreateUnaryOperator(Recreator &recreator)
{
    recreator.recreateUnaryOperator();
}

void recreateBinaryOperator(Recreator &recreator)
{
    recreator.recreateBinaryOperator();
}

void recreateNothing(Recreator &recreator)
{
    (void)recreator;
}
