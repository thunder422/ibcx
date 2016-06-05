/* vim:ts=4:sw=4:et:sts=4:
 *
 * Copyright 2016 Thunder422.  All rights reserved.
 * Distributed under GNU General Public License Version 3
 * (See accompanying file LICENSE or <http://www.gnu.org/licenses/>)
 */

#include "code.h"
#include "executer.h"

Executer::Executer(const WordType *code, const double *const_dbl_values,
        const int *const_int_values) :
    code {code},
    execute_functions {Code::getExecuteFunctions()},
    const_dbl_values {const_dbl_values},
    const_int_values {const_int_values},
    program_counter {const_cast<WordType *>(code)}
{
}

void Executer::executeOneCode()
{
    auto code_value = *program_counter++;
    execute_functions[code_value](*this);
}