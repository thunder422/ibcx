/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "commandcode.h"
#include "precedence.h"
#include "programcode.h"
#include "programerror.h"
#include "programunit.h"
#include "recreator.h"


Recreator::Recreator(const ProgramUnit &program, ProgramReader program_reader,
        unsigned error_offset) :
    program {program},
    program_reader {program_reader},
    error_offset {error_offset}
{
}

std::string &&Recreator::operator()()
{
    while (program_reader.hasMoreCode()) {
        setAtErrorOffset();
        recreateOneCode();
    }
    return topString();
}

void Recreator::setAtErrorOffset()
{
    at_error_offset = program_reader.currentOffset() == error_offset;
}

void Recreator::recreateOneCode()
{
    auto code = program_reader.getInstruction();
    code_value = code->getValue();
    code->recreate(*this);
}

std::string Recreator::getConstNumOperand() const
{
    auto operand = program_reader.getOperand();
    return program.constNumDictionary().get(operand);
}

void Recreator::pushKeyword(CommandCode command_code)
{
    push(command_code.getKeyword());
}

void Recreator::push(const std::string &operand)
{
    stack.emplace(operand, 0);
}

bool Recreator::empty() const
{
    return stack.empty();
}

std::string &&Recreator::topString()
{
    return std::move(stack.top().string);
}

unsigned Recreator::topPrecedence() const
{
    return stack.top().precedence;
}

bool Recreator::topUnaryOperator() const
{
    return stack.top().unary_operator;
}

void Recreator::pop()
{
    stack.pop();
}

void Recreator::prependKeyword(CommandCode command_code)
{
    std::string string = command_code.getKeyword();
    swapTop(string);
    append(' ');
    append(string);
}

void Recreator::append(char c)
{
    stack.top().string += c;
}

void Recreator::append(const std::string &string)
{
    stack.top().string += string;
}

void Recreator::swapTop(std::string &string)
{
    std::swap(stack.top().string, string);
}

void Recreator::setTopPrecedence(unsigned precedence)
{
    stack.top().precedence = precedence;
}

void Recreator::setTopUnaryOperator(bool unary_operator)
{
    stack.top().unary_operator = unary_operator;
}

void Recreator::markErrorStart()
{
    appendErrorMarker(StartErrorMarker);
}

void Recreator::appendErrorMarker(char error_marker)
{
    if (at_error_offset) {
        append(error_marker);
    }
}

void Recreator::recreateUnaryOperator()
{
    auto operand = topString();

    append(getOperatorKeyword());
    auto operator_precedence = getOperatorPrecedence();

    appendUnaryOperand(std::move(operand), operator_precedence);

    setTopPrecedence(operator_precedence);
    setTopUnaryOperator(true);
}

void Recreator::appendUnaryOperand(std::string &&operand, unsigned operator_precedence)
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

void Recreator::appendSpaceForConstant(char first_char)
{
    if (isdigit(first_char) || first_char == '.') {
        append(' ');
    }
}

void Recreator::recreateBinaryOperator()
{
    StackItem rhs = stack.top();
    pop();

    auto operator_precedence = getOperatorPrecedence();

    appendLeftOperand(operator_precedence);
    appendBinaryOperator();
    appendRightOperand(rhs, operator_precedence);

    setTopPrecedence(operator_precedence);
    setTopUnaryOperator(rhs.precedence > operator_precedence && rhs.unary_operator);
}

void Recreator::appendLeftOperand(unsigned operator_precedence)
{
    auto lhs_precedence = topPrecedence();
    auto lhs_unary_operator = topUnaryOperator();
    if (lhs_precedence > operator_precedence || lhs_unary_operator) {
        auto left_operand = topString();
        appendWithParens(left_operand);
    }
}

void Recreator::appendBinaryOperator()
{
    append(' ');
    markErrorStart();
    append(getOperatorKeyword());
    append(' ');
}

void Recreator::appendRightOperand(const StackItem &rhs, unsigned operator_precedence)
{
    auto lower_precedence = rhs.precedence >= operator_precedence && !rhs.unary_operator;
    if (lower_precedence) {
        appendWithParens(rhs.string);
    } else {
        append(rhs.string);
    }
}

void Recreator::appendWithParens(const std::string &string)
{
    append('(');
    append(string);
    append(')');
}

const char *Recreator::getOperatorKeyword() const
{
    return Precedence::getKeyword(code_value);
}

unsigned Recreator::getOperatorPrecedence() const
{
    return Precedence::getPrecedence(code_value);
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
