/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "executer.h"

Executer::Executer(const WordType *code, const int *const_int_values) :
    code {code},
    const_int_values {const_int_values},
    program_counter {const_cast<WordType *>(code)}
{
}

void Executer::executeOneCode()
{
    ++program_counter;
}

Executer::StackItem Executer::top()
{
    StackItem top;
    auto operand = *program_counter++;
    top.int_value = const_int_values[operand];
    return top;
}
