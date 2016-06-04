/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "executer.h"

Executer::Executer(const WordType *code, const int *const_int_values) :
    code {code},
    const_int_values {const_int_values},
    program_counter {const_cast<WordType *>(code)}
{
}

void Executer::executeOneCode()
{
    extern Code const_dbl_code;

    auto code = *program_counter++;
    if (code == const_dbl_code.getValue()) {
        stack.push(12.345);
    } else {
        auto operand = *program_counter++;
        stack.push(const_int_values[operand]);
    }
}
