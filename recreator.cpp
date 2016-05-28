/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "programcode.h"
#include "programunit.h"
#include "recreator.h"


Recreator::Recreator(ProgramUnit &program) :
    program {program},
    program_reader {program.createProgramReader()}
{
}

void Recreator::recreateOneCode()
{
    auto code = program_reader.getInstruction();
    code->recreate(*this);
}

std::string Recreator::getConstNumOperand()
{
    auto operand = program_reader.getOperand();
    return program.constNumDictionary().get(operand);
}

void Recreator::push(const std::string &operand)
{
    stack.emplace(operand);
}

std::string Recreator::top() const
{
    return stack.top().string;
}
