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
    return top();
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
    stack.emplace(operand);
}

bool Recreator::empty() const
{
    return stack.empty();
}

std::string Recreator::top() const
{
    return stack.top().string;
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

const char *Recreator::getOperatorKeyword() const
{
    return Precedence::getKeyword(code_value);
}

void Recreator::recreateUnaryOperator(const std::string &keyword)
{
    std::string string = keyword;
    swapTop(string);
    char c = string.front();
    if (isdigit(c) || c == '.') {
        append(' ');
    }
    append(string);
}

void Recreator::recreateBinaryOperator(const std::string &keyword)
{
    auto string = top();
    pop();
    append(' ');
    markErrorStart();
    append(keyword);
    append(' ');
    append(string);
}
