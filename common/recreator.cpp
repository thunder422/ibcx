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

std::string Recreator::operator()()
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

std::string Recreator::topString() const
{
    return stack.top().string;
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

void Recreator::setTopUnaryOperator()
{
    stack.top().unary_operator = true;
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
    std::string string = getOperatorKeyword();
    swapTop(string);
    char c = string.front();
    if (isdigit(c) || c == '.') {
        append(' ');
    }
    append(string);
    setTopPrecedence(getOperatorPrecedence());
    setTopUnaryOperator();
}

void Recreator::recreateBinaryOperator()
{
    auto right_operand = topString();
    auto right_precedence = topPrecedence();
    auto right_unary_operator = topUnaryOperator();
    pop();

    auto left_precedence = topPrecedence();
    auto operator_precedence = getOperatorPrecedence();
    if (left_precedence > operator_precedence) {
        std::string left_operand = "(";
        left_operand += topString();
        left_operand += ')';
        swapTop(left_operand);
    }
    append(' ');
    markErrorStart();
    append(getOperatorKeyword());
    append(' ');
    auto lower_precedence = right_precedence >= operator_precedence && !right_unary_operator;
    if (lower_precedence) {
        append('(');
    }
    append(right_operand);
    if (lower_precedence) {
        append(')');
    }
    setTopPrecedence(operator_precedence);
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
